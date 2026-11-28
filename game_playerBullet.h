/*============================================================================
Contents   :  [game_playerBullet.h]
              
Author     : Chin Qing You
LastUpdate : 2026/06/29
-----------------------------------------------------------------------------

============================================================================*/
#ifndef GAME_PLAYER_BULLET_H
#define GAME_PLAYER_BULLET_H

#include <DirectXMath.h>
#include "collision.h"

void GamePlayerBullet_Initialize();
void GamePlayerBullet_Finalize();

void GamePlayerBullet_Update(float delta_time);
void GamePlayerBullet_Draw();

void GamePlayerBullet_Create(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity);

int GamePlayerBullet_GetActiveCount();
void GamePlayerBullet_Destroy(int bulletIndex);
void GamePlayerBullet_CleanUp();
void GamePlayerBullet_Fire(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction);
bool GamePlayerBullet_IsDestroyed(int index);

CollisionAABB GamePlayerBullet_GetCollision(int bulletIndex);

#endif
