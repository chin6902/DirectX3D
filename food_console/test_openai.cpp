// Sends a list of foods to OpenAI, one by one, and prints one line each.
// Needs api_key.txt (one line, the key) in the working directory.
// Add your own words to the list to see how the AI rates them.
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "HttpClient.h"
#include "OpenAiApi.h"

string LoadApiKey(const string& path)
{
    ifstream file(path);
    string key;
    getline(file, key);
    while (!key.empty() && (key.back() == '\r' || key.back() == ' '))
    {
        key.pop_back();
    }
    return key;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    string apiKey = LoadApiKey("api_key.txt");
    if (apiKey.empty())
    {
        cout << "api_key.txt missing or empty (looked in the working directory)" << endl;
        return 1;
    }
    cout << "key loaded: " << apiKey.size() << " characters" << endl << endl;   // never print the key itself

    // Each test word with the kind we EXPECT, so a wrong answer stands out.
    // Careful: every line needs its comma. Two string literals with no comma
    // between them are silently joined into one ("miso soup" "ramen" -> "miso soupramen").
    struct Test { string word; string expect; string expectCuisine; };   // cuisine "" = not checked
    vector<Test> tests =
    {
        // one or two dishes from each cuisine
        { "ラーメン",                                    "dish", "japanese" },
        { "カレーライス",                                "dish", "japanese" },   // from India, but the Japanese form
        { "麻婆豆腐",                                    "dish", "chinese" },
        { "四川火鍋",                                    "dish", "chinese" },
        { "kimchi",                                     "dish", "korean" },
        { "bibimbap",                                   "dish", "korean" },
        { "tom yum",                                    "dish", "southeast_asian" },
        { "nasi lemak",                                 "dish", "southeast_asian" },
        { "pho",                                        "dish", "southeast_asian" },
        { "vindaloo",                                   "dish", "indian" },
        { "gulab jamun",                                "dish", "indian" },
        { "pizza",                                      "dish", "italian" },
        { "carbonara",                                  "dish", "italian" },
        { "ratatouille",                                "dish", "french" },
        { "hamburger",                                  "dish", "american" },
        // no listed cuisine -> other
        { "tacos",                                      "dish", "other" },
        { "hummus",                                     "dish", "other" },
        // still a dish, cuisine not checked (could fairly be japanese or other)
        { "shortcake",                                  "dish", "" },
        // refusals
        { "the spiciest ramen in the universe",         "extra_words", "" },
        { "ramen and sushi",                            "multiple", "" },
        { "chili",                                      "ingredient", "" },
        { "coffee",                                     "drink", "" },
        { "chair",                                      "not_food", "" },
    };

    wstring headers = BuildOpenAiHeaders(apiKey);

    int wrong   = 0;
    int totalMs = 0;

    for (const Test& test : tests)
    {
        const string& word = test.word;

        DWORD startMs = GetTickCount();
        HttpResult http = HttpPostJson(kOpenAiHost, kOpenAiPath, headers,
                                       BuildOpenAiRequestBody(word), 15000);
        DWORD tookMs = GetTickCount() - startMs;
        totalMs += (int)tookMs;

        cout << "\"" << word << "\"  (" << tookMs << " ms)" << endl;

        if (!http.ok)
        {
            cout << "    no reply: " << http.error << endl;
            continue;
        }

        FoodData food;
        string error;
        if (http.status == 200 && ParseOpenAiResponse(http.body, food, error))
        {
            if (food.isFood)
            {
                cout << "    OK  " << food.name << " [" << food.kind << ", " << food.cuisine << "]"
                     << "  sweet=" << food.sweet << " salty=" << food.salty
                     << " sour=" << food.sour << " spicy=" << food.spicy
                     << "  " << food.shape << "  with:";
                for (const string& tag : food.ingredients)
                {
                    cout << " " << tag;
                }
                cout << endl;
            }
            else
            {
                cout << "    REFUSED [" << food.kind << "]  " << RefusalReason(food) << endl;
            }
            cout << "        AI's reason: " << food.reason << endl;

            if (food.kind != test.expect)
            {
                cout << "    ^^^ UNEXPECTED: wanted " << test.expect << endl;
                wrong++;
            }
            else if (!test.expectCuisine.empty() && food.cuisine != test.expectCuisine)
            {
                cout << "    ^^^ UNEXPECTED cuisine: wanted " << test.expectCuisine << endl;
                wrong++;
            }
        }
        else
        {
            // Something went wrong: now the raw reply is worth reading.
            cout << "    FAILED status " << http.status << " " << error << endl;
            cout << "    " << http.body << endl;
            wrong++;
        }
    }

    cout << endl << tests.size() << " words, " << wrong << " unexpected, average "
         << totalMs / (int)tests.size() << " ms per word" << endl;
}
