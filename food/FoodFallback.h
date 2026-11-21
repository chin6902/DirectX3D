#pragma once
// ============================================================
// FoodFallback  (only used when the AI cannot be reached)
//
// 1. Find()     : hand-written, hand-checked foods (foods_fallback.json)
// 2. Generate() : any other word -> stats made from a HASH of the word
//
// The hash is the trick for "randomness without storage":
// the same word always gives the same numbers, but different words
// give different numbers, and it needs zero entries in a table.
// ============================================================
#include <unordered_map>
#include "FoodData.h"

class FoodFallback
{
public:
    void Load(const string& path);
    bool Find(const string& key, FoodData& out) const;
    FoodData Generate(const string& word) const;
    size_t Count() const { return m_foods.size(); }

private:
    unordered_map<string, FoodData> m_foods;
};

// FNV-1a: tiny, and gives the SAME number on every PC and compiler.
// (std::hash is allowed to differ between MSVC and other compilers.)
unsigned int HashString(const string& s);
