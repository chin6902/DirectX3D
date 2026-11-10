/*============================================================================
Contents   :  [collision3d_debug.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/10
-----------------------------------------------------------------------------

============================================================================*/
#ifndef COLLISION3D_DEBUG_H
#define COLLISION3D_DEBUG_H

#include "collision.h"

void Collision3D_Debug_Initialize();
void Collision3D_Debug_Finalize();

void Collision3D_Debug_Add(const CollisionAABB& aabb);
void Collision3D_Debug_Draw();

#endif
