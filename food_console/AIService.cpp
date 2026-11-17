#include "AIService.h"
#include "OpenAiApi.h"     // BuildOpenAiRequestBody, BuildOpenAiHeaders, ParseOpenAiResponse
#include "HttpClient.h"    // HttpPostJson
#include <chrono>
#include <fstream>

// Reads the first line of api_key.txt (same function as in test_openai.cpp).
static string LoadApiKey(const string& path)
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

void AIService::Start(const string& cachePath, const string& fallbackPath, const string& apiKeyPath)
{
    m_cache.Load(cachePath);
    m_fallback.Load(fallbackPath);
    m_apiKey = LoadApiKey(apiKeyPath);      // before the thread starts: no lock needed

    m_running = true;
    m_worker  = thread(&AIService::WorkerLoop, this);
}

void AIService::Stop()
{
    if (!m_running)
    {
        return;
    }
    {
        lock_guard<mutex> lock(m_mutex);
        m_running = false;
    }
    m_cv.notify_all();      // wake the worker so it can see m_running == false
    if (m_worker.joinable())
    {
        m_worker.join();    // wait for it to finish (max one HTTP timeout)
    }
}

// ---------------- MAIN THREAD ----------------

int AIService::Request(const string& word)
{
    string key = NormalizeKey(word);

    // Reject before spending money.
    // 60 bytes ~ 20 Japanese characters or 60 English letters.
    if (key.empty() || key.size() > 60)
    {
        return -1;
    }

    int id = m_nextId++;

    // 1) Cache hit: answer right away, no thread, no network.
    FoodData cached;
    if (m_cache.Find(key, cached))
    {
        m_ready.push_back({ id, word, cached, "" });
        return id;
    }

    // 2) Miss: give the job to the worker.
    {
        lock_guard<mutex> lock(m_mutex);
        m_jobs.push_back({ id, word, key });
    }
    m_pending++;
    m_cv.notify_one();
    return id;
}

bool AIService::PollResult(FoodResult& out)
{
    // Cache hits first.
    if (!m_ready.empty())
    {
        out = m_ready.front();
        m_ready.pop_front();
        return true;
    }

    // Take ONE finished response (if any). Hold the lock as briefly as possible.
    Response r;
    {
        lock_guard<mutex> lock(m_mutex);
        if (m_responses.empty())
        {
            return false;
        }
        r = m_responses.front();
        m_responses.pop_front();
    }
    m_pending--;

    out.id    = r.id;
    out.word  = r.word;
    out.error = r.error;

    if (r.ok)
    {
        // Real AI answer: save it (non-foods too, so "chair" never costs again).
        m_cache.Add(r.key, r.food);
        out.food = r.food;
    }
    else if (m_fallback.Find(r.key, out.food))
    {
        // Offline but it's a food we hand-wrote. NOT cached:
        // next time we're online the AI can still judge it properly.
    }
    else
    {
        out.food = m_fallback.Generate(r.word);   // also NOT cached
    }
    return true;
}

// ---------------- WORKER THREAD ----------------

void AIService::WorkerLoop()
{
    while (true)
    {
        Job job;
        {
            unique_lock<mutex> lock(m_mutex);
            // Sleep until there is a job or we are told to stop. Uses 0% CPU while waiting.
            m_cv.wait(lock, [this] { return !m_jobs.empty() || !m_running; });
            if (!m_running)
            {
                return;
            }
            job = m_jobs.front();
            m_jobs.pop_front();
        }   // unlock BEFORE the slow call, or the game would freeze in Request()

        Response r;
        r.id   = job.id;
        r.word = job.word;
        r.key  = job.key;
        r.ok   = CallBackend(job.word, r.food, r.error);

        {
            lock_guard<mutex> lock(m_mutex);
            m_responses.push_back(r);
        }
    }
}

bool AIService::CallBackend(const string& word, FoodData& out, string& error)
{
    if (m_offline)
    {
        this_thread::sleep_for(chrono::milliseconds(300));  // pretend to time out
        error = "offline";
        return false;
    }

    if (m_apiKey.empty())
    {
        error = "no API key (api_key.txt missing or empty)";
        return false;
    }

    // ------------------------------------------------------------
    // YOUR TURN: connect the real AI.
    //
    // Delete the three lines under this comment and write the same
    // four steps that test_openai.cpp does, in the same order:
    //
    //   1. string body = BuildOpenAiRequestBody(word);
    //   2. wstring headers = BuildOpenAiHeaders(m_apiKey);
    //   3. HttpResult http = HttpPostJson(kOpenAiHost, kOpenAiPath, headers, body, 15000);
    //        no reply at all (!http.ok)   -> error = http.error;                       return false;
    //        reply, but not status 200    -> error = "HTTP " + to_string(http.status); return false;
    //   4. return ParseOpenAiResponse(http.body, out, error);
    //
    // Returning false is not a crash: PollResult() then uses the fallback.
    // Until you write this, EVERY word comes back as FALLBACK or GENERATED.
    // ------------------------------------------------------------
    /*(void)word;
    (void)out;
    error = "CallBackend is not connected yet";*/
    return false;
}
