/*============================================================================
Contents   :  [game_explosion.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/24
-----------------------------------------------------------------------------

============================================================================*/
#ifndef GAME_EXPLOSION_H
#define GAME_EXPLOSION_H

#include <DirectXMath.h>

void GameExplosion_Initialize();
void GameExplosion_Finalize();
void GameExplosion_Update(float delta_time);
void GameExplosion_Draw();

void GameExplosion_Spawn(const DirectX::XMFLOAT3& position);

#endif
