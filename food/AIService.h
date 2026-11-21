#pragma once
// ============================================================
// AIService  — the bridge between the game loop and the AI.
//
//  MAIN THREAD (game)                 WORKER THREAD
//  ------------------                 -------------
//  Request("ramen")
//    cache hit?  -> m_ready (instant)
//    miss        -> m_requests ──────> wait for work
//                                      send HTTP  (1-2 s, blocks here,
//                                                  NOT in your game loop)
//                                      parse reply
//                   m_responses <───── push result
//  PollResult()  every frame
//    validate, save to cache,
//    or use fallback if it failed
//    -> hand FoodData to the game
//
// Rule: the worker only does slow I/O + pure parsing.
// Cache, fallback, and all game state stay on the main thread,
// so they never need locks.
// ============================================================
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <atomic>
#include "FoodData.h"
#include "FoodCache.h"
#include "FoodFallback.h"

struct FoodResult
{
    int      id = 0;        // matches the id Request() returned
    string   word;          // what the player typed
    FoodData food;
    string   error;         // why the AI failed (empty if it worked)
};

class AIService
{
public:
    ~AIService() { Stop(); }

    void Start(const string& cachePath, const string& fallbackPath, const string& apiKeyPath);
    void Stop();

    int  Request(const string& word);   // returns request id, or -1 if rejected
    bool PollResult(FoodResult& out);   // call every frame until it returns false
    int  PendingCount() const { return m_pending; }

    void SetOffline(bool offline) { m_offline = offline; }   // test the fallback path
    const FoodCache& Cache() const { return m_cache; }

private:
    struct Job
    {
        int    id;
        string word;
        string key;
    };
    struct Response     // raw result from the worker
    {
        int      id;
        string   word;
        string   key;
        bool     ok;
        FoodData food;
        string   error;
    };

    void WorkerLoop();
    bool CallBackend(const string& word, FoodData& out, string& error);   // worker thread

    // shared between threads -> only touch while holding m_mutex
    mutex              m_mutex;
    condition_variable m_cv;
    deque<Job>         m_jobs;
    deque<Response>    m_responses;

    // simple flags, atomic is enough
    atomic<bool> m_running { false };
    atomic<bool> m_offline { false };
    atomic<int>  m_pending { 0 };

    // main thread only
    thread             m_worker;
    deque<FoodResult>  m_ready;     // cache hits waiting to be polled
    FoodCache          m_cache;
    FoodFallback       m_fallback;
    int                m_nextId = 1;

    // Written once in Start() BEFORE the worker thread exists, then only read.
    // That is why the worker may read it without a lock.
    string             m_apiKey;
};
