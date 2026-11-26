/*============================================================================
Contents   :  [game_map.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/12s
-----------------------------------------------------------------------------
todo: refactor with class later
============================================================================*/
#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "collision.h"
#include "map_object.h"

void GameMap_Initialize();
void GameMap_Finalize();
void GameMap_Update(float delta_time);
void GameMap_Draw();

HitData3D GameMap_CheckCollision(const CollisionAABB& aabb);

int GameMap_GetActiveCount();
const MapObject* GameMap_GetObject(int index);
DirectX::XMFLOAT3 GameMap_GetFloorMove(const CollisionAABB& aabb);

#endif
