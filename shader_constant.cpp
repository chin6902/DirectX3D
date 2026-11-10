/*============================================================================
Contents   : [shader_constant.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/09
-----------------------------------------------------------------------------

============================================================================*/
#include <DirectXMath.h>

#include "shader_constant.h"
#include "direct3d.h"

using namespace DirectX;

struct WorldBuffer
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 worldIT;
};

void ShaderConstant::Initialize()
{
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// b0 : world + worldIT
	buffer_desc.ByteWidth = sizeof(WorldBuffer);          // 128 bytes
	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &m_pVSConstantBuffer[0]);

	// b1 : view, b2 : projection
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4);           // 64 bytes
	for (unsigned int i = 1; i < VS_CONSTANT_BUFFER_COUNT; i++)
	{
		Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &m_pVSConstantBuffer[i]);
	}

	// PS b1 : color (b0 is used for light)
	buffer_desc.ByteWidth = sizeof(XMFLOAT4);             // 16 bytes
	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &m_pPSConstantBuffer[0]);
}

void ShaderConstant::Finalize()
{
	SAFE_RELEASE(m_pVSConstantBuffer[0]);
	SAFE_RELEASE(m_pVSConstantBuffer[1]);
	SAFE_RELEASE(m_pVSConstantBuffer[2]);
	SAFE_RELEASE(m_pPSConstantBuffer[0]);
}

void ShaderConstant::SetWorldMatrix(const DirectX::XMMATRIX & matrix)
{
	WorldBuffer wb{};

	// both transposed for HLSL
	XMStoreFloat4x4(&wb.world, XMMatrixTranspose(matrix));
	XMStoreFloat4x4(&wb.worldIT, XMMatrixInverse(nullptr, matrix));

	// 定数バッファに行列をセット
	Direct3D_GetDeviceContext()->UpdateSubresource(m_pVSConstantBuffer[0], 0, nullptr, &wb, 0, 0);
}

void ShaderConstant::SetViewMatrix(const DirectX::XMMATRIX & matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	Direct3D_GetDeviceContext()->UpdateSubresource(m_pVSConstantBuffer[1], 0, nullptr, &transpose, 0, 0);
}

void ShaderConstant::SetProjectionMatrix(const DirectX::XMMATRIX & matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	Direct3D_GetDeviceContext()->UpdateSubresource(m_pVSConstantBuffer[2], 0, nullptr, &transpose, 0, 0);
}

void ShaderConstant::SetColor(const DirectX::XMFLOAT4 & color)
{
	// 定数バッファに色をセット
	Direct3D_GetDeviceContext()->UpdateSubresource(m_pPSConstantBuffer[0], 0, nullptr, &color, 0, 0);
}

void ShaderConstant::Bind()
{
	// 定数バッファを描画パイプラインに設定
	Direct3D_GetDeviceContext()->VSSetConstantBuffers(0, VS_CONSTANT_BUFFER_COUNT, m_pVSConstantBuffer);

	Direct3D_GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_pPSConstantBuffer[0]);
}

static ShaderConstant g_ShaderConstant3D;

ShaderConstant& ShaderConstant_Get3D()
{
	return g_ShaderConstant3D;
}


