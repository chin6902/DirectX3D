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

struct BillboardDrawParams
{
	DirectX::XMFLOAT2 size{ 1.0f, 1.0f };
	DirectX::XMFLOAT2 pivot{ 0.5f, 0.5f };   
	DirectX::XMFLOAT4 uv_rect{ 0.0f, 0.0f, 1.0f, 1.0f };  // u, v, width, height
};

inline constexpr DirectX::XMFLOAT2 BILLBOARD_PIVOT_CENTER{ 0.5f, 0.5f };
inline constexpr DirectX::XMFLOAT2 BILLBOARD_PIVOT_BOTTOM{ 0.5f, 1.0f };

void Billboard_Initialize();
void Billboard_Finalize();
void Billboard_SetViewMatrix(const DirectX::XMMATRIX& view);

void Billboard_Draw(int texture_id, const DirectX::XMFLOAT3& position);
void Billboard_Draw(int texture_id, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size);
void Billboard_Draw(int texture_id, const DirectX::XMFLOAT3& position, const BillboardDrawParams& params);

void Billboard_Flush();

#endif
