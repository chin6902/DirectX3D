#pragma once
// ============================================================
// FoodCache
// A map  key -> FoodData  that is kept in memory while the game
// runs AND saved to a JSON file, so it survives between sessions.
//
//   game start : Load()  file -> memory
//   new food   : Add()   memory + write file
//   lookup     : Find()  memory only (fast, no disk)
//
// Main thread only. The AI worker thread never touches this.
// ============================================================
#include <unordered_map>
#include "FoodData.h"

class FoodCache
{
public:
    void Load(const string& path);
    bool Find(const string& key, FoodData& out) const;
    void Add(const string& key, const FoodData& food);
    size_t Count() const { return m_foods.size(); }

private:
    void Save() const;

    string m_path;
    unordered_map<string, FoodData> m_foods;
};
