#include <cstdio>

#include "BattleRules.h"

static const float kDishMultiplier[MAX_SERVE + 1] = { 0.0f, 3.0f, 1.5f, 1.0f };
static const float kWrongCuisineScale = 0.5f;

static const Order kOrders[] =
{
    // sweet salty sour spicy   ingredient   cuisine             seconds damage attack
    { {  0,   0,    0,   6 },   "",          "",                 45.0f, 10.0f, 1.0f },
    { {  8,   0,    0,   0 },   "",          "",                 45.0f, 10.0f, 1.0f },
    { {  0,   6,    6,   0 },   "",          "",                 45.0f, 10.0f, 1.0f },
    { {  0,   0,    0,  10 },   "",          "",                 45.0f, 10.0f, 1.0f },
    { {  0,  10,    0,   0 },   "noodles",   "",                 50.0f, 10.0f, 1.0f },
    { { 10,   0,    3,   0 },   "",          "",                 50.0f, 10.0f, 1.0f },
    { {  0,   0,    0,   8 },   "",          "chinese",          50.0f, 15.0f, 1.0f },
    { {  8,   0,    0,   0 },   "",          "japanese",         50.0f, 15.0f, 1.0f },
    { {  0,   0,    0,  12 },   "seafood",   "southeast_asian",  60.0f, 15.0f, 1.0f },
    { {  0,   8,    8,   8 },   "garlic",    "",                 60.0f, 15.0f, 1.0f },
};

static const int kOrderCount = sizeof(kOrders) / sizeof(kOrders[0]);
static const char* const kTasteNames[TASTE_COUNT] = { "sweet", "salty", "sour", "spicy" };

const char* TasteName(int taste)
{
	if (taste < 0 || taste >= TASTE_COUNT)
	{
		return "?";
	}

	return kTasteNames[taste];
}

int OrderCount()
{
    return kOrderCount;
}

const Order& GetOrder(int index)
{
	index = index % kOrderCount;

	if (index < 0)
	{
		index += kOrderCount;
	}

	return kOrders[index];
}

Taste TasteOf(const FoodData& food)
{
    Taste t;
	t.value[TASTE_SWEET] = food.sweet; 
	t.value[TASTE_SALTY] = food.salty; 
	t.value[TASTE_SOUR] = food.sour; 
	t.value[TASTE_SPICY] = food.spicy;

	return t;
}

Taste AddTaste(const FoodData* foods[], int count)
{
    Taste total;

    for(int i = 0; i < count; i++)
    {
        Taste t = TasteOf(*foods[i]);
        for(int j = 0; j < TASTE_COUNT; j++)
        {
            total.value[j] += t.value[j];
        }
    }

    return total;
}

float DishMultiplier(int count)
{
	if (count < 1 || count > MAX_SERVE)
	{
		return 0.0f;
	}

	return kDishMultiplier[count];
}

ServeResult JudgeServe(const Order& order, const FoodData* foods[], int count)
{
    ServeResult r;

	if (count < 1 || count > MAX_SERVE)
	{
		return r;
	}

	r.total = AddTaste(foods, count);
	r.met = true;

	for (int i = 0; i < TASTE_COUNT; i++)
	{
		r.tasteOK[i] = (r.total.value[i] >= order.need.value[i]);
		if (!r.tasteOK[i])
		{
			r.met = false;
		}
	}

	if (order.cuisine[0] != '\0')
	{
		for (int i = 0; i < count; i++)
		{
			if (foods[i]->cuisine != order.cuisine)
			{
				r.cuisineOK = false;
			}
		}
	}

	if (order.ingredient[0] != '\0')
	{
		r.ingredientOK = false;
		for (int i = 0; i < count; i++)
		{
			if (HasIngredient(*foods[i], order.ingredient))
			{
				r.ingredientOK = true;
			}
		}
		if (!r.ingredientOK)
		{
			r.met = false;
		}
	}

	if (r.met)
	{
		r.damage = order.damage * DishMultiplier(count);

		if (!r.cuisineOK)
		{
			r.damage *= kWrongCuisineScale;
		}
	}

	return r;
}

string ServeMessage(const Order& order, const ServeResult& result, const FoodData* foods[], int count)
{
    char buf[256];

    if (result.met)
    {
        snprintf(buf, sizeof(buf), "Hit! %d damage (%d dish%s x%g)",
            (int)result.damage, count, count == 1 ? "" : "es", DishMultiplier(count));
        string text = buf;

        if (!result.cuisineOK)
        {
            // Name the first dish with the wrong cuisine, so the player
            // sees what the AI thought.
            for (int i = 0; i < count; i++)
            {
                if (foods[i]->cuisine != order.cuisine)
                {
                    snprintf(buf, sizeof(buf), "  Half damage: AI says %s is %s, not %s.",
                        foods[i]->name.c_str(), foods[i]->cuisine.c_str(), order.cuisine);
                    text += buf;
                    break;
                }
            }
        }
        return text;
    }

    if (count < 1)
    {
        return "Select 1 to 3 dishes first.";
    }

    // A miss: list every taste that was too low, then the ingredient.
    string text = "Missed!";
    for (int k = 0; k < TASTE_COUNT; k++)
    {
        if (!result.tasteOK[k])
        {
            snprintf(buf, sizeof(buf), "  %s %d / %d", TasteName(k),
                result.total.value[k], order.need.value[k]);
            text += buf;
        }
    }
    if (!result.ingredientOK)
    {
        snprintf(buf, sizeof(buf), "  Needs %s: AI says none of these has it.", order.ingredient);
        text += buf;
    }
    return text;
}
