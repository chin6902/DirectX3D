/*============================================================================
Contents   :  [light.h]

Author     : Chin Qing You
LastUpdate : 2026/09/30
-----------------------------------------------------------------------------

============================================================================*/
#include "light.h"
#include "direct3d.h"

using namespace DirectX;

static ID3D11Buffer* g_pPSConstantBuffer0 = nullptr;

struct Light
{
	XMFLOAT4 directionW;
	XMFLOAT4 ambientColor;
};

void Light_Initialize()
{
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(Light); 
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer0);
}

void Light_Finalize()
{
	SAFE_RELEASE(g_pPSConstantBuffer0);
}

void Light_SetDirectionalLight(const DirectX::XMFLOAT3 & directionW, const DirectX::XMFLOAT4 & ambientColor)
{
	Light light{};
	// light vector normalization
	XMVECTOR dir{ XMVector3Normalize(XMLoadFloat3(&directionW)) };
	// Store the normalized direction in the light struct
	XMStoreFloat4(&light.directionW, dir);
	
	// Set the ambient color
	light.ambientColor = ambientColor;

	Direct3D_GetDeviceContext()->UpdateSubresource(g_pPSConstantBuffer0, 0, nullptr, &light, 0, 0);
	Direct3D_GetDeviceContext()->PSSetConstantBuffers(0, 1, &g_pPSConstantBuffer0);
}
