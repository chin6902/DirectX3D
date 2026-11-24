/*============================================================================
Contents   :  [game_battle.h]

Author     : Chin Qing You
LastUpdate : 2026/11/23
-----------------------------------------------------------------------------

============================================================================*/
#ifndef GAME_BATTLE_H
#define GAME_BATTLE_H

#include "health.h"

struct FoodData;	
struct Order;		

const int INVENTORY_SIZE = 5;

void Battle_Initialize();
void Battle_Finalize();

void Battle_Update(float delta_time);

bool Battle_Serve(const int slots[], int count);

bool Battle_IsWon();		// boss HP 0
bool Battle_IsLost();		// player HP 0

const Order& Battle_GetOrder();
int   Battle_GetOrderNumber();		
float Battle_GetOrderTimeLeft();		
const Health& Battle_GetBossHealth();
const Health& Battle_GetPlayerHealth();
bool  Battle_IsSlotUsed(int slot);
const FoodData& Battle_GetSlot(int slot);		
const char* Battle_GetMessage();			

#endif // GAME_BATTLE_H