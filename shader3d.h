/*============================================================================
Contents   : [shader3d.h]
              
Author     : Chin Qing You
LastUpdate : 2026/09/14
-----------------------------------------------------------------------------

============================================================================*/
#ifndef SHADER3D_H
#define SHADER3D_H

#include <d3d11.h>
#include <DirectXMath.h>

bool Shader3d_Initialize();
void Shader3d_Finalize();

void Shader3d_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader3d_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Shader3d_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

void Shader3d_Begin();

#endif
