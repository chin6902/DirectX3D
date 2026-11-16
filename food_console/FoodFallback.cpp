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

FoodData FoodFallback::Generate(const string& word) const
{
    unsigned int h = HashString(NormalizeKey(word));

    // Take different bits of the hash for each stat.
    FoodData f;
    f.name  = word;
    f.kind  = "dish";               // offline we can't check, so be generous...
    f.sweet = (h      ) % 7;        // ...but keep stats LOW (0-6) so typing
    f.salty = (h >>  8) % 7;        // random words offline is never better
    f.sour  = (h >> 16) % 7;        // than a real, AI-checked food
    f.spicy = (h >> 24) % 7;
                                    // no ingredients: offline unknown words
                                    // can't answer an ingredient order

    const char* shapes[] = { "bowl", "plate", "sphere", "stick", "block", "cup" };
    f.shape = shapes[(h >> 4) % 6];

    SanitizeFood(f);
    f.source = FoodSource::Generated;
    return f;
}
