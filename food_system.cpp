/*============================================================================
Contents   :  [food_system.h]

Author     : Chin Qing You
LastUpdate : 2026/11/21
-----------------------------------------------------------------------------

============================================================================*/
#include <Windows.h>	

#include <cstdio>
#include <deque>
#include <string>
#include <vector>

#include "food_system.h"
#include "AIService.h"		

static const char* kCachePath = "foods_cache.json";
static const char* kFallbackPath = "food/foods_fallback.json";
static const char* kApiKeyPath = "api_key.txt";

static const size_t kMaxResults = 20;	// how many finished results are kept
static const size_t kMaxPending = 3;	// how many words may wait for the AI at once

struct PendingWord
{
	int    id;
	string word;	// as the player typed it
	string key;		// NormalizeKey(word), to spot the same word sent twice
};

static AIService           g_AI;
static bool                g_Started = false;
static deque<FoodResult>   g_Results;	// newest first
static vector<PendingWord> g_Pending;	// oldest first

#ifdef _DEBUG
// One line per result in Visual Studio's Output window.
// Names are left out: Japanese text sent with OutputDebugStringA looks broken there.
static void DebugPrintResult(const FoodResult& res)
{
	const FoodData& f = res.food;

	char line[256];
	snprintf(line, sizeof(line),
		"[food] #%d %s kind=%s sweet=%d salty=%d sour=%d spicy=%d %s\n",
		res.id, SourceName(f.source), f.kind.c_str(),
		f.sweet, f.salty, f.sour, f.spicy,
		res.error.empty() ? "" : ("| AI not used: " + res.error).c_str());
	OutputDebugStringA(line);
}
#endif

void FoodSystem_Initialize()
{
	if (g_Started)
	{
		return;
	}

	g_AI.Start(kCachePath, kFallbackPath, kApiKeyPath);
	g_Started = true;
}

void FoodSystem_Finalize()
{
	if (!g_Started)
	{
		return;
	}

	g_AI.Stop();		// joins the worker thread
	g_Results.clear();
	g_Pending.clear();
	g_Started = false;
}

void FoodSystem_Update()
{
	if (!g_Started)
	{
		return;
	}

	// The same loop as in the console main.cpp.
	FoodResult res;
	while (g_AI.PollResult(res))
	{
		// This word is no longer waiting.
		for (size_t i = 0; i < g_Pending.size(); i++)
		{
			if (g_Pending[i].id == res.id)
			{
				g_Pending.erase(g_Pending.begin() + i);
				break;
			}
		}

		g_Results.push_front(res);
		if (g_Results.size() > kMaxResults)
		{
			g_Results.pop_back();
		}

#ifdef _DEBUG
		DebugPrintResult(res);
#endif
	}
}

int FoodSystem_Request(const char* word)
{
	if (!g_Started || word == nullptr)
	{
		return FOOD_REQUEST_REJECTED;
	}

	// Empty (or only spaces) or too long: say so before anything else.
	// (AIService::Request has the same 60-byte rule; this keeps the answer
	//  "rejected" even while other words are waiting.)
	string key = NormalizeKey(word);
	if (key.empty() || key.size() > 60)
	{
		return FOOD_REQUEST_REJECTED;
	}

	// The same word is already being judged
	for (const PendingWord& p : g_Pending)
	{
		if (p.key == key)
		{
			return p.id;
		}
	}

	if (g_Pending.size() >= kMaxPending)
	{
		return FOOD_REQUEST_BUSY;
	}

	int id = g_AI.Request(word);	// returns at once; -1 = empty or too long
	if (id < 0)
	{
		return FOOD_REQUEST_REJECTED;
	}

	g_Pending.push_back({ id, word, key });
	return id;
}

bool FoodSystem_IsBusy()
{
	return !g_Pending.empty();
}

int FoodSystem_GetPendingCount()
{
	return (int)g_Pending.size();
}

const char* FoodSystem_GetPendingWord(int index)
{
	if (index < 0 || index >= (int)g_Pending.size())
	{
		return "";
	}
	return g_Pending[index].word.c_str();
}

int FoodSystem_GetResultCount()
{
	return (int)g_Results.size();
}

const FoodResult& FoodSystem_GetResult(int index)
{
	static const FoodResult empty;		// returned for a wrong index, so the caller never crashes

	if (index < 0 || index >= (int)g_Results.size())
	{
		return empty;
	}
	return g_Results[index];
}
