#pragma once
// ============================================================
// FoodData.h
// The ONE struct the whole game uses for a food.
// Where it came from (AI, cache, fallback table...) does not
// matter to battle code. Battle code only ever sees a FoodData.
// ============================================================
#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Where this FoodData came from. Useful for debug UI and for
// deciding whether a result is allowed to go into the cache.
enum class FoodSource
{
    Cache,      // found in foods_cache.json (instant)
    AI,         // just came back from the AI (then saved to cache)
    Fallback,   // offline: found in the hand-written table
    Generated,  // offline: unknown word, stats made from a hash
    Rejected    // input was refused before sending (too long, empty...)
};

struct FoodData
{
    string         name;                 // display name
    string         kind   = "not_food";  // one of FoodKinds()
    bool           isFood = false;       // can it be served? OUR code decides this from kind
    int            sweet  = 0;           // 0-10  (0 = this taste is absent)
    int            salty  = 0;           // 0-10
    int            sour   = 0;           // 0-10
    int            spicy  = 0;           // 0-10
    vector<string> ingredients;          // tags from IngredientTags(), e.g. "garlic"
    string         shape  = "plate";
    string         texturePath;          // empty until an image exists
    FoodSource     source = FoodSource::Rejected;
};

// The two fixed lists. Each exists in ONE place: the AI request (schema)
// and SanitizeFood both read them from here, so they can never disagree.
const vector<string>& FoodKinds();        // dish, drink, ingredient, invented, not_food
const vector<string>& IngredientTags();   // garlic, onion, egg, ...

bool        HasIngredient(const FoodData& f, const string& tag);

int         ClampInt(int v, int lo, int hi);
string      NormalizeKey(const string& word);   // "  Ramen " -> "ramen"
void        SanitizeFood(FoodData& f);          // clamp + fix + decide isFood
json        FoodToJson(const FoodData& f);
bool        FoodFromJson(const json& j, FoodData& out);
const char* SourceName(FoodSource s);
