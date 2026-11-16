#include "FoodCache.h"
#include <fstream>
#include <filesystem>

// Bump this whenever the rules for the stats change (range, new fields...).
// Files with another version are ignored, so old numbers never mix with new ones.
//   1 = stats 1-10      2 = stats 0-10
//   3 = savory renamed salty, kind + ingredients added, grade removed
static const int kCacheVersion = 3;

void FoodCache::Load(const string& path)
{
    m_path = path;
    m_foods.clear();

    ifstream file(path);
    if (!file)
    {
        return;     // first run: no file yet, that's fine
    }

    try
    {
        json root = json::parse(file);
        if (!root.is_object() || root.value("version", 0) != kCacheVersion || !root.contains("foods"))
        {
            return;     // old format: start empty, the AI will be asked again
        }
        for (auto& [key, value] : root["foods"].items())
        {
            FoodData food;
            if (FoodFromJson(value, food))
            {
                m_foods[key] = food;
            }
        }
    }
    catch (const json::exception&)
    {
        // Broken file (game crashed mid-write, hand edit typo...).
        // Start empty instead of crashing the game.
        m_foods.clear();
    }
}

bool FoodCache::Find(const string& key, FoodData& out) const
{
    auto it = m_foods.find(key);
    if (it == m_foods.end())
    {
        return false;
    }
    out = it->second;
    out.source = FoodSource::Cache;
    return true;
}

void FoodCache::Add(const string& key, const FoodData& food)
{
    m_foods[key] = food;
    Save();     // small file, so just rewrite it every time
}

void FoodCache::Save() const
{
    json root;
    root["version"] = kCacheVersion;
    root["foods"]   = json::object();
    for (auto& [key, food] : m_foods)
    {
        root["foods"][key] = FoodToJson(food);
    }

    // Write to a temp file first, then rename over the real one.
    // If the game crashes during the write, the old cache is still intact.
    string tempPath = m_path + ".tmp";
    {
        ofstream file(tempPath);
        file << root.dump(2);   // dump(2) = pretty print, easy to read and edit
    }
    std::error_code ec;
    filesystem::rename(tempPath, m_path, ec);
}
