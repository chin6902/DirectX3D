// Tests the battle rules (BattleRules.h) without the AI and without the game.
// Part 1: hand-made dishes, one line per rule, PASS or FAIL.
// Part 2: can every order be met with the offline table (foods_fallback.json)?
// Switch between main.cpp, test_openai.cpp and this file with
// Properties -> Excluded From Build (only one may be built at a time).
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include "BattleRules.h"

static int g_Failures = 0;

// A dish made by hand. The stats do not have to be what the AI would say;
// they only have to make each test easy to follow.
static FoodData Dish(const string& name, int sweet, int salty, int sour, int spicy,
    const string& cuisine, const vector<string>& ingredients)
{
    FoodData f;
    f.name = name;
    f.kind = "dish";
    f.sweet = sweet;
    f.salty = salty;
    f.sour = sour;
    f.spicy = spicy;
    f.cuisine = cuisine;
    f.ingredients = ingredients;
    SanitizeFood(f);    // same clamps and isFood rule as real dishes
    return f;
}

static void Check(const string& name, bool ok)
{
    cout << (ok ? "PASS  " : "FAIL  ") << name << endl;
    if (!ok)
    {
        g_Failures++;
    }
}

static Order MakeOrder(int sweet, int salty, int sour, int spicy, const char* ingredient, const char* cuisine)
{
    Order o;
    o.need.value[TASTE_SWEET] = sweet;
    o.need.value[TASTE_SALTY] = salty;
    o.need.value[TASTE_SOUR] = sour;
    o.need.value[TASTE_SPICY] = spicy;
    o.ingredient = ingredient;
    o.cuisine = cuisine;
    o.damage = 10.0f;
    return o;
}

static void TestRules()
{
    FoodData hotpot = Dish("sichuan hot pot", 0, 7, 0, 10, "chinese", { "beef", "chili", "garlic" });
    FoodData mapo = Dish("mapo tofu", 1, 6, 0, 7, "chinese", { "tofu", "pork", "chili", "garlic" });
    FoodData curry = Dish("curry", 3, 5, 0, 4, "japanese", { "onion", "potato", "rice" });
    FoodData ramen = Dish("ramen", 1, 6, 0, 1, "japanese", { "noodles", "pork", "egg" });

    Order spicy10 = MakeOrder(0, 0, 0, 10, "", "");

    {
        const FoodData* foods[] = { &mapo, &curry };
        ServeResult r = JudgeServe(spicy10, foods, 2);
        Check("mapo tofu + curry (spicy 11) meets spicy 10", r.met);
        Check("2 dishes do 10 x 1.5 = 15", r.damage == 15.0f);
        cout << "      " << ServeMessage(spicy10, r, foods, 2) << endl;
    }
    {
        const FoodData* foods[] = { &hotpot };
        ServeResult r = JudgeServe(spicy10, foods, 1);
        Check("1 hot pot meets spicy 10 and does 10 x 3 = 30", r.met && r.damage == 30.0f);
    }
    {
        const FoodData* foods[] = { &hotpot, &mapo, &curry };
        ServeResult r = JudgeServe(spicy10, foods, 3);
        Check("3 dishes do 10 x 1 = 10", r.met && r.damage == 10.0f);
    }
    {
        const FoodData* foods[] = { &mapo };
        ServeResult r = JudgeServe(spicy10, foods, 1);
        Check("mapo tofu alone (spicy 7) misses spicy 10", !r.met && r.damage == 0.0f);
        Check("the miss marks spicy as too low", !r.tasteOK[TASTE_SPICY] && r.tasteOK[TASTE_SWEET]);
        cout << "      " << ServeMessage(spicy10, r, foods, 1) << endl;
    }
    {
        Order japaneseSpicy = MakeOrder(0, 0, 0, 10, "", "japanese");
        const FoodData* foods[] = { &hotpot };
        ServeResult r = JudgeServe(japaneseSpicy, foods, 1);
        Check("wrong cuisine is still a hit, at half damage (15)", r.met && !r.cuisineOK && r.damage == 15.0f);
        cout << "      " << ServeMessage(japaneseSpicy, r, foods, 1) << endl;
    }
    {
        Order japaneseSalty = MakeOrder(0, 10, 0, 0, "", "japanese");
        const FoodData* foods[] = { &ramen, &curry };
        ServeResult r = JudgeServe(japaneseSalty, foods, 2);
        Check("every dish japanese = full damage", r.met && r.cuisineOK && r.damage == 15.0f);

        const FoodData* mixed[] = { &ramen, &mapo };
        r = JudgeServe(japaneseSalty, mixed, 2);
        Check("one chinese dish among them = half damage", r.met && !r.cuisineOK && r.damage == 7.5f);
    }
    {
        Order garlicSalty = MakeOrder(0, 5, 0, 0, "garlic", "");
        const FoodData* foods[] = { &ramen };
        ServeResult r = JudgeServe(garlicSalty, foods, 1);
        Check("missing garlic is a miss, even with salty reached", !r.met && !r.ingredientOK && r.tasteOK[TASTE_SALTY]);
        cout << "      " << ServeMessage(garlicSalty, r, foods, 1) << endl;

        const FoodData* withGarlic[] = { &ramen, &mapo };
        r = JudgeServe(garlicSalty, withGarlic, 2);
        Check("garlic in one of two dishes is enough", r.met && r.ingredientOK);
    }
    {
        const FoodData* foods[] = { &hotpot };
        Check("0 dishes is never a hit", !JudgeServe(spicy10, foods, 0).met);
        Check("4 dishes is never a hit", !JudgeServe(spicy10, foods, 4).met);
    }
}

// Tries every way to pick 1 to 3 dishes from the table (the same dish may be
// picked twice, the player can type it twice) and keeps the best damage.
static void TestOrdersOffline(const string& path)
{
    ifstream file(path);
    if (!file)
    {
        cout << "FAIL  could not open " << path << endl;
        g_Failures++;
        return;
    }

    vector<FoodData> dishes;
    json root = json::parse(file);
    for (auto& [key, value] : root["foods"].items())
    {
        FoodData f;
        if (FoodFromJson(value, f))
        {
            SanitizeFood(f);
            if (f.isFood)
            {
                dishes.push_back(f);
            }
        }
    }
    cout << endl << dishes.size() << " dishes in the offline table" << endl;

    const int n = (int)dishes.size();
    for (int o = 0; o < OrderCount(); o++)
    {
        const Order& order = GetOrder(o);
        float best = 0.0f;
        string bestNames;

        for (int a = 0; a < n; a++)
        {
            for (int b = -1; b < n; b++)            // -1 = no second dish
            {
                for (int c = -1; c < n; c++)        // -1 = no third dish
                {
                    if (b < 0 && c >= 0)
                    {
                        continue;                   // same as a pick with b, skip it
                    }
                    const FoodData* foods[MAX_SERVE];
                    int count = 0;
                    foods[count++] = &dishes[a];
                    if (b >= 0) { foods[count++] = &dishes[b]; }
                    if (c >= 0) { foods[count++] = &dishes[c]; }

                    ServeResult r = JudgeServe(order, foods, count);
                    if (r.damage > best)
                    {
                        best = r.damage;
                        bestNames.clear();
                        for (int i = 0; i < count; i++)
                        {
                            bestNames += (i > 0 ? " + " : "") + foods[i]->name;
                        }
                    }
                }
            }
        }

        bool ok = (best > 0.0f);
        Check("order " + to_string(o + 1) + " can be met offline"
            + (ok ? " (best " + to_string((int)best) + ": " + bestNames + ")" : ""), ok);
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    TestRules();
    TestOrdersOffline("../food/foods_fallback.json");

    cout << endl << (g_Failures == 0 ? "All tests passed." : to_string(g_Failures) + " FAILED") << endl;
    return g_Failures == 0 ? 0 : 1;
}