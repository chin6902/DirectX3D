#include "FoodFallback.h"
#include <fstream>

unsigned int HashString(const string& s)
{
    unsigned int h = 2166136261u;
    for (unsigned char c : s)
    {
        h ^= c;
        h *= 16777619u;
    }
    return h;
}

void FoodFallback::Load(const string& path)
{
    m_foods.clear();

    ifstream file(path);
    if (!file)
    {
        return;
    }

    try
    {
        json root = json::parse(file);
        for (auto& [key, value] : root["foods"].items())
        {
            FoodData food;
            if (FoodFromJson(value, food))
            {
                m_foods[NormalizeKey(key)] = food;      // kind comes from the file
            }
        }
    }
    catch (const json::exception&)
    {
        m_foods.clear();
    }
}

bool FoodFallback::Find(const string& key, FoodData& out) const
{
    auto it = m_foods.find(key);
    if (it == m_foods.end())
    {
        return false;
    }
    out = it->second;
    out.source = FoodSource::Fallback;
    return true;
}

// What to do with a word that the AI could not judge AND the table does not know.
//   false = refuse it. Without the AI we can't tell "chair" from a real dish.
//   true  = accept it with low stats made from a hash of the word (the old
//           behaviour: the player always gets something, but "chair" becomes a dish).
static const bool kAcceptUnknownOffline = false;

FoodData FoodFallback::Generate(const string& word) const
{
    FoodData f;
    f.name = word;
    f.source = FoodSource::Generated;

    if (!kAcceptUnknownOffline)
    {
        f.kind = "offline";         // SanitizeFood: not a dish -> refused, stats 0
        SanitizeFood(f);
        return f;
    }

    // Take different bits of the hash for each stat (Lesson 7).
    unsigned int h = HashString(NormalizeKey(word));
    f.kind = "dish";
    f.sweet = (h) % 7;        // low stats (0-6), so a random word offline
    f.salty = (h >> 8) % 7;        // is never better than a real, AI-checked dish
    f.sour = (h >> 16) % 7;
    f.spicy = (h >> 24) % 7;

    const char* shapes[] = { "bowl", "plate", "sphere", "stick", "block", "cup" };
    f.shape = shapes[(h >> 4) % 6];

    SanitizeFood(f);
    f.source = FoodSource::Generated;
    return f;
}