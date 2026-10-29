/*============================================================================
Contents   :  [game_player.h]
              
Author     : Chin Qing You
LastUpdate : 2026/10/21
-----------------------------------------------------------------------------

============================================================================*/
#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <DirectXMath.h>

void GamePlayer_Initialize(const DirectX::XMFLOAT3& position, float angle_y);
void GamePlayer_Finalize();

void GamePlayer_Update(float delta_time);
void GamePlayer_Draw();

const DirectX::XMFLOAT3& GamePlayer_GetPosition();

#endif
