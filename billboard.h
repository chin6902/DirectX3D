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

void Billboard_Initialize(ShaderConstant* pShaderConstant);
void Billboard_Finalize();
void Billboard_SetViewMatrix(const DirectX::XMMATRIX& view);
void Billboard_Draw(const DirectX::XMMATRIX& world);

#endif
