/*============================================================================
Contents   :  [food_system.h]

Author     : Chin Qing You
LastUpdate : 2026/11/21
-----------------------------------------------------------------------------

============================================================================*/
#ifndef FOOD_SYSTEM_H
#define FOOD_SYSTEM_H

struct FoodResult;		// defined in AIService.h (id, word, food, error)

const int FOOD_REQUEST_REJECTED = -1;	// empty, or longer than 60 bytes
const int FOOD_REQUEST_BUSY = -2;		// too many words are still being judged

void FoodSystem_Initialize();	// loads the cache, the fallback table and the API key, starts the worker thread
void FoodSystem_Finalize();		// stops the worker thread (waits for a running request to end or time out)

// Call once per frame, on the main thread (Application_Update).
// Collects finished results from the worker.
void FoodSystem_Update();

// Sends a word to be judged. Returns at once; the result arrives in a later frame.
int  FoodSystem_Request(const char* word);

// Words that were sent and have not come back yet.
bool FoodSystem_IsBusy();
int FoodSystem_GetPendingCount();
const char* FoodSystem_GetPendingWord(int index);	// 0 = oldest

// Finished results, newest first (index 0 = newest). At most 20 are kept.
int FoodSystem_GetResultCount();
const FoodResult& FoodSystem_GetResult(int index);

#endif // FOOD_SYSTEM_H