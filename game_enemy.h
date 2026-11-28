/*============================================================================
Contents   :  [game_enemy.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/26
-----------------------------------------------------------------------------
remake the enemy system
============================================================================*/
#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <DirectXMath.h>
#include "enemy_manager.h"

void GameEnemy_Initialize();
void GameEnemy_Finalize();

void GameEnemy_Register(GameEnemy* enemy);
void GameEnemy_Update(float delta_time);
void GameEnemy_Draw();

void GameEnemy_Destroy(int index);
void GameEnemy_CleanUp();

CollisionAABB GameEnemy_GetCollision(int index);

#endif