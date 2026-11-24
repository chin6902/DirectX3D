#ifndef BATTLE_RULES_H
#define BATTLE_RULES_H

#include "FoodData.h"

const int MAX_SERVE = 3;

enum TasteIndex
{
	TASTE_SWEET,
	TASTE_SALTY,
	TASTE_SOUR,
	TASTE_SPICY,
	TASTE_COUNT
};

struct Taste
{
	int value[TASTE_COUNT]{}; 
};

struct Order
{
	Taste		need;
	const char* ingredient = "";
	const char* cuisine = "";
	float		seconds = 45.0f;
	float		damage = 10.0f;
	float		attack = 1.0f;
};

struct ServeResult
{
	Taste total;
	bool  tasteOK[TASTE_COUNT] = {};
	bool  ingredientOK = true;
	bool  cuisineOK = true;
	bool  met = false;
	float damage = 0.0f;
};

const char* TasteName(int taste);

int OrderCount();
const Order& GetOrder(int index);

Taste TasteOf(const FoodData& food);
Taste AddTaste(const FoodData* foods[], int count);
float DishMultiplier(int count);

ServeResult JudgeServe(const Order& order, const FoodData* foods[], int count);

string ServeMessage(const Order& order, const ServeResult& result, const FoodData* foods[], int count);

#endif
