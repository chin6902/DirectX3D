/*============================================================================
Contents   :  [billboard.h]
              
Author     : Chin Qing You
LastUpdate : 2026/11/17
-----------------------------------------------------------------------------

============================================================================*/
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <DirectXMath.h>
#include "shader_constant.h"

void Billboard_Initialize();
void Billboard_Finalize();
void Billboard_SetViewMatrix(const DirectX::XMMATRIX& view);
void Billboard_Draw(int texture_id, const DirectX::XMFLOAT3& position);
void Billboard_Flush();

#endif
