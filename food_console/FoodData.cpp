#include "FoodData.h"
#include <cctype>

// ---------------- the two fixed lists ----------------
// "static" inside a function = created once, the first time the function
// is called, and kept for the rest of the program.

const vector<string>& FoodKinds()
{
    static const vector<string> kinds =
    {
        "dish",         // ONE real prepared dish, typed as just its name -> the ONLY kind that can be served
        "drink",        // a real beverage
        "ingredient",   // raw ingredient or seasoning on its own (chili, lemon, soy sauce)
        "invented",     // a mix of foods that is not an established dish
        "multiple",     // two or more separate foods in one input (ramen and sushi)
        "extra_words",  // a real food with words added to its name (the spiciest ramen in the universe)
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

// Shortens a UTF-8 string to at most maxBytes WITHOUT cutting a character in half.
// (Lesson 2: substr counts bytes. Half a Japanese character is broken UTF-8,
//  and json's dump() throws on that when the cache is saved.)
// The 2nd and 3rd bytes of a UTF-8 character are always 128-191, so if the byte
// at the cut is in that range we are inside a character: step back.
string CutUtf8(const string& s, size_t maxBytes)
{
    if (s.size() <= maxBytes)
    {
        return s;
    }

    size_t cut = maxBytes;
    while (cut > 0 && (unsigned char)s[cut] >= 128 && (unsigned char)s[cut] <= 191)
    {
        cut--;
    }
    return s.substr(0, cut);
}

// NEVER trust what the AI sends back. Everything goes through here.
void SanitizeFood(FoodData& f)
{
    f.sweet = ClampInt(f.sweet, 0, 10);
    f.salty = ClampInt(f.salty, 0, 10);
    f.sour = ClampInt(f.sour, 0, 10);
    f.spicy = ClampInt(f.spicy, 0, 10);

    // Only allow shapes we actually have meshes for.
    const vector<string> shapes = { "bowl", "plate", "sphere", "stick", "block", "cup" };
    if (!Contains(shapes, f.shape))
    {
        f.shape = "plate";
    }

    f.name = CutUtf8(f.name, 60);
    f.reason = CutUtf8(f.reason, 120);

    // Unknown kind -> treat as not food.
    // "offline" is the one extra kind. It is never sent by the AI (it is not in
    // FoodKinds()): our own code sets it when the AI could not be reached and
    // the word is not in the hand-written table.
    if (!Contains(FoodKinds(), f.kind) && f.kind != "offline")
    {
        f.kind = "not_food";
    }

    // THE RULE: only a real dish can be served.
    // A drink, a bare ingredient ("chili") or an invented mix is refused.
    // This is a game rule, so our code decides it, not the AI.
    f.isFood = (f.kind == "dish");

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
        f.sour = 0;
        f.spicy = 0;
        f.ingredients.clear();
    }
}

json FoodToJson(const FoodData& f)
{
    json j;
    j["name"] = f.name;
    j["kind"] = f.kind;
    j["reason"] = f.reason;
    j["sweet"] = f.sweet;
    j["salty"] = f.salty;
    j["sour"] = f.sour;
    j["spicy"] = f.spicy;
    j["ingredients"] = f.ingredients;       // vector<string> -> JSON array
    j["shape"] = f.shape;
    j["texture"] = f.texturePath;
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
        out.name = j.value("name", "");
        out.kind = j.value("kind", "not_food");
        out.reason = j.value("reason", "");
        out.sweet = j.value("sweet", 0);
        out.salty = j.value("salty", 0);
        out.sour = j.value("sour", 0);
        out.spicy = j.value("spicy", 0);
        out.shape = j.value("shape", "plate");
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

// What to tell the player when a word is refused.
// One place for these texts, so the console test and the game show the same thing.
const char* RefusalReason(const FoodData& f)
{
    if (f.kind == "drink")       return "That's a drink. Serve a dish!";
    if (f.kind == "ingredient")  return "That's an ingredient, not a dish.";
    if (f.kind == "invented")    return "Nobody makes that. Name a real dish.";
    if (f.kind == "multiple")    return "One dish at a time!";
    if (f.kind == "extra_words") return "Just the name of the dish, nothing extra.";
    if (f.kind == "not_food")    return "That's not food!";
    if (f.kind == "offline")     return "The AI chef can't be reached. Try a famous dish.";
    return "";                   // "dish": not refused
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