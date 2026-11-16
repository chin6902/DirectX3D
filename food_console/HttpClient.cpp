// Windows headers FIRST: they must be read before any "using namespace std",
// or std::byte and Windows' own byte collide ("reference to 'byte' is ambiguous").
#include <windows.h>
#include <winhttp.h>
#include "HttpClient.h"

#pragma comment(lib, "winhttp.lib")     // MSVC: link WinHTTP automatically

HttpResult HttpPostJson(const wstring& host, const wstring& path,
                        const wstring& headers, const string& body, int timeoutMs)
{
    HttpResult res;

    // 1. Session: "open the phone app". The name is just how we introduce ourselves.
    HINTERNET session = WinHttpOpen(L"FoodBattle/0.1",
                                    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session)
    {
        res.error = "WinHttpOpen failed: " + to_string(GetLastError());
        return res;
    }

    // Never wait forever: resolve, connect, send, receive (milliseconds).
    WinHttpSetTimeouts(session, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

    // 2. Connection: "dial the company". 443 = the HTTPS port.
    HINTERNET connect = WinHttpConnect(session, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);

    // 3. Request: "ask for the department". WINHTTP_FLAG_SECURE = HTTPS (encrypted).
    HINTERNET request = nullptr;
    if (connect)
    {
        request = WinHttpOpenRequest(connect, L"POST", path.c_str(), nullptr,
                                     WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     WINHTTP_FLAG_SECURE);
    }

    if (!request)
    {
        res.error = "could not open request: " + to_string(GetLastError());
    }
    else
    {
        // 4. Send headers + body. (DWORD)-1 = "headers end at the first \0".
        BOOL sent = WinHttpSendRequest(request, headers.c_str(), (DWORD)-1,
                                       (LPVOID)body.data(), (DWORD)body.size(),
                                       (DWORD)body.size(), 0);

        // 5. Wait for the reply to start arriving (this is where the 1-2 s goes).
        if (sent && WinHttpReceiveResponse(request, nullptr))
        {
            // 6. Status code: 200, 401, 429...
            DWORD status = 0;
            DWORD size   = sizeof(status);
            WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &size,
                                WINHTTP_NO_HEADER_INDEX);
            res.status = (int)status;

            // 7. Read the body. It can arrive in several chunks, so loop.
            DWORD available = 0;
            while (WinHttpQueryDataAvailable(request, &available) && available > 0)
            {
                string chunk(available, '\0');
                DWORD  read = 0;
                if (!WinHttpReadData(request, &chunk[0], available, &read))
                {
                    break;
                }
                chunk.resize(read);
                res.body += chunk;
            }
            res.ok = true;
        }
        else
        {
            // 12002 = timeout, 12007 = name not resolved (no internet), 12029 = can't connect
            res.error = "send/receive failed: " + to_string(GetLastError());
        }
    }

    // 8. Hang up, in reverse order of opening (like destructors, Lesson 4).
    if (request)
    {
        WinHttpCloseHandle(request);
    }
    if (connect)
    {
        WinHttpCloseHandle(connect);
    }
    WinHttpCloseHandle(session);
    return res;
}
