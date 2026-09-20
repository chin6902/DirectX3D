/*============================================================================
Contents   :  [cube.h]
              
Author     : Chin Qing You
LastUpdate : 2026/09/14
-----------------------------------------------------------------------------

============================================================================*/
#ifndef CUBE_H
#define CUBE_H

#include <DirectXMath.h>

void Cube_Initialize();
void Cube_Finalize();
void Cube_Update(float delta_time);
void Cube_Draw(const DirectX::XMMATRIX& world);

#endif
