#pragma once
// ============================================================
// OpenAiApi: the same two jobs as ClaudeApi, for OpenAI.
//
//   BuildOpenAiRequestBody(word)       -> JSON text we POST
//   ParseOpenAiResponse(body, out)     -> FoodData from the reply
//
// Nothing else in the game knows which AI company is used.
// ============================================================
#include "FoodData.h"

// Where the request goes and how we log in (used by the HTTP call).
extern const wchar_t* kOpenAiHost;      // api.openai.com
extern const wchar_t* kOpenAiPath;      // /v1/chat/completions
wstring BuildOpenAiHeaders(const string& apiKey);

string BuildOpenAiRequestBody(const string& word);
bool   ParseOpenAiResponse(const string& body, FoodData& out, string& error);
