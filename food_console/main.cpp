// ============================================================
// Console test of the whole food pipeline.
// The loop below is shaped like your game's Update():
// it never waits for the AI, it just checks for results each frame.
//
// Run it TWICE: the second run shows the cache surviving between sessions.
// Needs foods_fallback.json and api_key.txt in the working directory.
// ============================================================
#ifdef _WIN32
#include <windows.h>        // first: must come before any "using namespace std"
#endif
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
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);    // so the console can show Japanese
#endif

    AIService ai;
    ai.Start("foods_cache.json", "../food/foods_fallback.json", "api_key.txt");
    printf("Cache has %zu foods from earlier sessions\n\n", ai.Cache().Count());

    vector<FoodData> inventory;     // later: your bar UI at the bottom of the screen
    const size_t kMaxSlots = 5;

    Enemy boss = { "Chili Golem", 200, 0, 0, 0, 7, "" };    // needs spicy >= 7, any ingredient

    // What the "player" types, one word after another.
    struct Input { string word; bool goOffline; };
    vector<Input> inputs =
    {
        { "Ramen",            false },
        { "麻婆豆腐",          false },
        { "chair",            false },     // not food
        { "chili",            false },     // an ingredient, not a dish
        { "",                 false },     // rejected before sending
        { "  RAMEN ",         false },     // same key as "Ramen" -> cache
        { "pizza",            true  },     // offline -> hand-written table
        { "dragonfruit soda", true  },     // offline -> stats from the hash
    };

    size_t nextInput = 0;
    auto   sentTime  = chrono::steady_clock::now();

    // 6000 frames = about 100 seconds. Only a safety limit: the loop
    // normally ends as soon as every word has come back.
    for (int frame = 0; frame < 6000; frame++)
    {
        // --- player input: type the next word once the previous one is back ---
        if (nextInput < inputs.size() && ai.PendingCount() == 0)
        {
            const Input& in = inputs[nextInput++];
            ai.SetOffline(in.goOffline);
            sentTime = chrono::steady_clock::now();

            int id = ai.Request(in.word);
            if (id < 0)
            {
                printf("[frame %4d] \"%s\" rejected before sending\n", frame, in.word.c_str());
            }
            else
            {
                printf("[frame %4d] request #%d \"%s\"\n", frame, id, in.word.c_str());
            }
        }

        // --- results (this is the part that goes in your Update) ---
        FoodResult res;
        while (ai.PollResult(res))
        {
            const FoodData& f = res.food;
            int tookMs = (int)chrono::duration_cast<chrono::milliseconds>(
                chrono::steady_clock::now() - sentTime).count();

            printf("[frame %4d] #%d %-9s %s [%s, %s]  sweet=%d salty=%d sour=%d spicy=%d  %s  with:",
                frame, res.id, SourceName(f.source), f.name.c_str(), f.kind.c_str(), f.cuisine.c_str(),
                f.sweet, f.salty, f.sour, f.spicy, f.shape.c_str());
            for (const string& tag : f.ingredients)
            {
                printf(" %s", tag.c_str());
            }
            printf("   (%d ms)\n", tookMs);

            // FALLBACK or GENERATED means CallBackend returned false. Always say why.
            if (f.source == FoodSource::Fallback || f.source == FoodSource::Generated)
            {
                printf("             AI failed: %s\n",
                    res.error.empty() ? "(CallBackend returned false but did not set error)" : res.error.c_str());
            }

            if (!f.isFood)
            {
                printf("             -> refused [%s]: %s\n", f.kind.c_str(), RefusalReason(f));
                continue;
            }
            if (inventory.size() < kMaxSlots)
            {
                inventory.push_back(f);
            }
            if (CanDamage(f, boss))
            {
                printf("             -> vs %s: HIT for %d\n", boss.name.c_str(), CalcDamage(f));
            }
            else
            {
                printf("             -> vs %s: no effect (needs spicy 7)\n", boss.name.c_str());
            }
        }

        if (nextInput == inputs.size() && ai.PendingCount() == 0)
        {
            break;      // everything was typed and everything came back
        }
        this_thread::sleep_for(chrono::milliseconds(16));   // ~60 fps
    }

    printf("\nInventory: %zu foods. Cache now has %zu foods (saved to foods_cache.json)\n",
        inventory.size(), ai.Cache().Count());
    ai.Stop();
    return 0;
}
