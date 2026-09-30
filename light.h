/*============================================================================
Contents   :  [light.h]
              
Author     : Chin Qing You
LastUpdate : 2026/09/30
-----------------------------------------------------------------------------

============================================================================*/
#ifndef LIGHT_H
#define LIGHT_H

#include <DirectXMath.h>

void Light_Initialize();
void Light_Finalize();

void Light_SetDirectionalLight(const DirectX::XMFLOAT3& directionW, const DirectX::XMFLOAT4& ambientColor);

#endif
