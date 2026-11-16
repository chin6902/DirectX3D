// ============================================================
// Console test of the whole food pipeline.
// The loop below is shaped like your game's Update():
// it never waits for the AI, it just checks for results each frame.
// Run it TWICE: the second run shows the cache surviving between sessions.
// ============================================================
#include <cstdio>
#include <vector>
#include <chrono>
#include "AIService.h"

// ---------------- game side: using the generated data ----------------

struct Enemy
{
    string name;
    int    hp;
    int    needSweet, needSalty, needSour, needSpicy;   // 0 = no requirement
    string needIngredient;                               // "" = any
};

bool CanDamage(const FoodData& f, const Enemy& e)
{
    if (!e.needIngredient.empty() && !HasIngredient(f, e.needIngredient))
    {
        return false;
    }
    return f.sweet >= e.needSweet
        && f.salty >= e.needSalty
        && f.sour  >= e.needSour
        && f.spicy >= e.needSpicy;
}

int CalcDamage(const FoodData& f)
{
    return f.sweet + f.salty + f.sour + f.spicy;
}

// ---------------- test driver ----------------

int main()
{
    AIService ai;
    ai.Start("foods_cache.json", "foods_fallback.json");
    printf("Cache has %zu foods from earlier sessions\n\n", ai.Cache().Count());

    vector<FoodData> inventory;     // later: your bar UI at the bottom of the screen
    const size_t kMaxSlots = 5;

    Enemy boss = { "Chili Golem", 200, 0, 0, 0, 7, "" };    // needs spicy >= 7, any ingredient

    // What the "player" types, and on which frame.
    struct Input { int frame; string word; bool goOffline; };
    vector<Input> inputs =
    {
        {   0, "Ramen",            false },
        {   0, "麻婆豆腐",          false },
        {  10, "chair",            false },
        {  20, "",                 false },     // rejected: empty
        { 150, "  RAMEN ",         false },     // same key as "Ramen"
        { 160, "pizza",            true  },     // offline -> hand-written table
        { 160, "dragonfruit soda", true  },     // offline -> hash generated
    };

    auto startTime = chrono::steady_clock::now();
    size_t nextInput = 0;

    for (int frame = 0; frame < 400; frame++)
    {
        // --- player input ---
        while (nextInput < inputs.size() && inputs[nextInput].frame == frame)
        {
            const Input& in = inputs[nextInput++];
            ai.SetOffline(in.goOffline);
            int id = ai.Request(in.word);
            if (id < 0)
            {
                printf("[frame %3d] \"%s\" rejected before sending\n", frame, in.word.c_str());
            }
            else
            {
                printf("[frame %3d] request #%d \"%s\"  (start minigame here)\n", frame, id, in.word.c_str());
            }
        }

        // --- results (this is the part that goes in your Update) ---
        FoodResult res;
        while (ai.PollResult(res))
        {
            const FoodData& f = res.food;
            float sec = chrono::duration<float>(chrono::steady_clock::now() - startTime).count();
            printf("[frame %3d | %.2fs] #%d %-9s %-16s %-10s swe%d sal%d sou%d spi%d  %s",
                frame, sec, res.id, SourceName(f.source), f.name.c_str(), f.kind.c_str(),
                f.sweet, f.salty, f.sour, f.spicy, f.shape.c_str());
            if (!res.error.empty())
            {
                printf("   (AI failed: %s)", res.error.c_str());
            }
            printf("\n");

            if (!f.isFood)
            {
                printf("            -> refused (%s)\n", f.kind.c_str());
                continue;
            }
            if (inventory.size() < kMaxSlots)
            {
                inventory.push_back(f);
            }
            printf("            -> vs %s: %s\n", boss.name.c_str(),
                CanDamage(f, boss) ? ("HIT for " + to_string(CalcDamage(f))).c_str() : "no effect (needs spicy 7)");
        }

        this_thread::sleep_for(chrono::milliseconds(16));   // ~60 fps
        if (nextInput == inputs.size() && ai.PendingCount() == 0 && frame > 170)
        {
            break;
        }
    }

    printf("\nInventory: %zu foods. Cache now has %zu foods (saved to foods_cache.json)\n",
        inventory.size(), ai.Cache().Count());
    ai.Stop();
    return 0;
}
