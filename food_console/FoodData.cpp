#include "FoodData.h"
#include <cctype>

// ---------------- the two fixed lists ----------------
// "static" inside a function = created once, the first time the function
// is called, and kept for the rest of the program.

const vector<string>& FoodKinds()
{
    static const vector<string> kinds =
    {
        "dish",         // a real prepared dish or snack   -> can be served
        "drink",        // a real beverage                 -> can be served
        "ingredient",   // raw ingredient or seasoning on its own (chili, lemon, soy sauce)
        "invented",     // a mix of foods that is not an established dish
        "not_food"      // everything else (chair, a sentence...)
    };
    return kinds;
}

// Boss orders can ask for one of these. To add a tag, add it here:
// the AI request and the checks pick it up automatically.
// (Changing this list changes what cached foods mean -> bump the cache version.)
const vector<string>& IngredientTags()
{
    static const vector<string> tags =
    {
        "garlic", "onion", "egg", "cheese", "milk", "rice",
        "noodles", "bread", "pork", "beef", "chicken", "seafood",
        "tofu", "tomato", "potato", "chili", "fruit", "chocolate"
    };
    return tags;
}

static bool Contains(const vector<string>& list, const string& s)
{
    for (const string& item : list)
    {
        if (item == s)
        {
            return true;
        }
    }
    return false;
}

bool HasIngredient(const FoodData& f, const string& tag)
{
    return Contains(f.ingredients, tag);
}

int ClampInt(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// The cache key. "Ramen", "ramen", "  RAMEN  " must all be the same food,
// otherwise the cache misses and you pay for the same food again.
// Only ASCII is lowercased: bytes >= 0x80 are parts of Japanese (UTF-8)
// characters and must not be touched.
string NormalizeKey(const string& word)
{
    string out;
    bool lastWasSpace = true;   // true so leading spaces are skipped

    for (unsigned char c : word)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            if (!lastWasSpace)
            {
                out += ' ';
            }
            lastWasSpace = true;
            continue;
        }

        if (c < 0x80)
        {
            out += (char)tolower(c);
        }
        else
        {
            out += (char)c;
        }
        lastWasSpace = false;
    }

    if (!out.empty() && out.back() == ' ')
    {
        out.pop_back();
    }
    return out;
}

// NEVER trust what the AI sends back. Everything goes through here.
void SanitizeFood(FoodData& f)
{
    f.sweet = ClampInt(f.sweet, 0, 10);
    f.salty = ClampInt(f.salty, 0, 10);
    f.sour  = ClampInt(f.sour,  0, 10);
    f.spicy = ClampInt(f.spicy, 0, 10);

    // Only allow shapes we actually have meshes for.
    const vector<string> shapes = { "bowl", "plate", "sphere", "stick", "block", "cup" };
    if (!Contains(shapes, f.shape))
    {
        f.shape = "plate";
    }

    if (f.name.size() > 60)
    {
        f.name = f.name.substr(0, 60);
    }

    // Unknown kind -> treat as not food.
    if (!Contains(FoodKinds(), f.kind))
    {
        f.kind = "not_food";
    }

    // THE RULE: only a real dish or drink can be served.
    // A bare ingredient ("chili") or an invented mix is refused.
    // This is a game rule, so our code decides it, not the AI.
    f.isFood = (f.kind == "dish" || f.kind == "drink");

    // Keep only known tags, no duplicates, at most 8.
    vector<string> clean;
    for (const string& tag : f.ingredients)
    {
        if (Contains(IngredientTags(), tag) && !Contains(clean, tag) && clean.size() < 8)
        {
            clean.push_back(tag);
        }
    }
    f.ingredients = clean;

    // Something that can't be served has no stats at all,
    // so nothing can leak into the battle by accident.
    if (!f.isFood)
    {
        f.sweet = 0;
        f.salty = 0;
        f.sour  = 0;
        f.spicy = 0;
        f.ingredients.clear();
    }
}

json FoodToJson(const FoodData& f)
{
    json j;
    j["name"]        = f.name;
    j["kind"]        = f.kind;
    j["sweet"]       = f.sweet;
    j["salty"]       = f.salty;
    j["sour"]        = f.sour;
    j["spicy"]       = f.spicy;
    j["ingredients"] = f.ingredients;       // vector<string> -> JSON array
    j["shape"]       = f.shape;
    j["texture"]     = f.texturePath;
    return j;
}

// Returns false instead of crashing when the JSON is broken
// (missing field, "8" as a string instead of a number, etc).
bool FoodFromJson(const json& j, FoodData& out)
{
    try
    {
        if (!j.is_object())
        {
            return false;
        }
        out.name        = j.value("name", "");
        out.kind        = j.value("kind", "not_food");
        out.sweet       = j.value("sweet", 0);
        out.salty       = j.value("salty", 0);
        out.sour        = j.value("sour", 0);
        out.spicy       = j.value("spicy", 0);
        out.shape       = j.value("shape", "plate");
        out.texturePath = j.value("texture", "");

        out.ingredients.clear();
        if (j.contains("ingredients") && j["ingredients"].is_array())
        {
            for (const json& tag : j["ingredients"])
            {
                if (tag.is_string())
                {
                    out.ingredients.push_back(tag.get<string>());
                }
            }
        }

        SanitizeFood(out);
        return true;
    }
    catch (const json::exception&)
    {
        return false;
    }
}

const char* SourceName(FoodSource s)
{
    switch (s)
    {
    case FoodSource::Cache:     return "CACHE";
    case FoodSource::AI:        return "AI";
    case FoodSource::Fallback:  return "FALLBACK";
    case FoodSource::Generated: return "GENERATED";
    case FoodSource::Rejected:  return "REJECTED";
    }
    return "?";
}
