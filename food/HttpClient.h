#pragma once
// ============================================================
// HttpClient: one function that sends an HTTPS POST and returns
// the reply. Windows only (WinHTTP). Blocking: call it from the
// worker thread, never from the game loop.
// ============================================================
#include <string>

using namespace std;

struct HttpResult
{
    bool   ok     = false;  // true = we got a reply (any status code)
    int    status = 0;      // 200, 400, 401, 429, 529...
    string body;            // the reply text (JSON)
    string error;           // when ok == false: what went wrong
};

HttpResult HttpPostJson(const wstring& host, const wstring& path,
                        const wstring& headers, const string& body, int timeoutMs);
