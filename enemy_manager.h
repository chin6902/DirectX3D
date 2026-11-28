/*============================================================================
Contents   :  [enemy_manager.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/26
-----------------------------------------------------------------------------

============================================================================*/
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include <DirectXMath.h>

class GameEnemy;

enum EnemyType
{
	kEnemyType_Slime,
	kEnemyType_Max,
};

void EnemyManager_Initialize();
void EnemyManager_Finalize();

void EnemyManager_Update(float delta_time);
void EnemyManager_Draw();
void EnemyManager_CleanUp();

GameEnemy* EnemyManager_Spawn(EnemyType type, const DirectX::XMFLOAT3& position);

int EnemyManager_GetCount();
GameEnemy* EnemyManager_Get(int index);

#endif
