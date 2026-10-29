/*============================================================================
Contents   :  [mesh_field.cpp]

Author     : Chin Qing You
LastUpdate : 2026/10/20
-----------------------------------------------------------------------------
// use a common rasterizer state and depth stencil state(class)
============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>

#include "mesh_field.h"
#include "config.h"
#include "direct3d.h"
#include "shader3d.h"
#include "debug_ostream.h"
#include "texture.h"

using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer{ nullptr };
static ID3D11Buffer* g_pIndexBuffer{ nullptr };

static ID3D11RasterizerState*	g_pRasterizerState{ nullptr };
static ID3D11DepthStencilState* g_pDepthStencilState{ nullptr };

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

static int g_NumIndex{ 0 };

void MeshField_Initialize(float grid_width, float grid_depth, int h_count, int v_count)
{
	const int NUM_VERTEX_H = h_count + 1;
	const int NUM_VERTEX_V = v_count + 1;
	const int NUM_VERTEX = NUM_VERTEX_H * NUM_VERTEX_V;

	g_NumIndex = h_count * v_count * 6;

	if (h_count <= 0 || v_count <= 0 || NUM_VERTEX > 65536) return;

	D3D11_BUFFER_DESC bd{
		.ByteWidth = sizeof(Vertex) * NUM_VERTEX,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0
	};

	Vertex* pV = new Vertex[NUM_VERTEX];

	const float START_X = grid_width * h_count * -0.5f;
	const float START_Z = grid_depth * v_count * 0.5f;

	for (int z = 0; z < NUM_VERTEX_V; z++)
	{
		for (int x = 0; x < NUM_VERTEX_H; x++)
		{
			const int INDEX = z * NUM_VERTEX_H + x;
			const float POS_X = START_X + grid_width * x;
			const float POS_Z = START_Z - grid_depth * z;
			pV[INDEX].position = {POS_X, 0.0f, POS_Z};
			pV[INDEX].normal = { 0.0f, 1.0f, 0.0f };
			pV[INDEX].color = { 1.0f, 1.0f, 1.0f, 1.0f };
			pV[INDEX].uv = { (float)x, (float)z };
		}
	}

	D3D11_SUBRESOURCE_DATA sd{
		.pSysMem = pV
	};

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	delete[] pV;

	D3D11_BUFFER_DESC ibd{
		.ByteWidth = sizeof(unsigned short) * g_NumIndex,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0
	};

	unsigned short* pI = new unsigned short[g_NumIndex];

	// pI index data 
	int n = 0;
	for (int z = 0; z < v_count; z++)
	{
		for (int x = 0; x < h_count; x++)
		{
			if (n + 6 > g_NumIndex) break;  

			const unsigned short v0 = (unsigned short)(z * NUM_VERTEX_H + x);
			const unsigned short v1 = (unsigned short)(z * NUM_VERTEX_H + x + 1);
			const unsigned short v2 = (unsigned short)((z + 1) * NUM_VERTEX_H + x);
			const unsigned short v3 = (unsigned short)((z + 1) * NUM_VERTEX_H + x + 1);

			pI[n++] = v0; pI[n++] = v1; pI[n++] = v2;  // triangle 1
			pI[n++] = v1; pI[n++] = v3; pI[n++] = v2;  // triangle 2
		}
	}

	sd.pSysMem = pI;
	Direct3D_GetDevice()->CreateBuffer(&ibd, &sd, &g_pIndexBuffer);

	delete[] pI;

	D3D11_DEPTH_STENCIL_DESC dsd{};
	dsd.DepthEnable = TRUE;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Direct3D_GetDevice()->CreateDepthStencilState(&dsd, &g_pDepthStencilState);

	D3D11_RASTERIZER_DESC rd{};
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	Direct3D_GetDevice()->CreateRasterizerState(&rd, &g_pRasterizerState);
	Direct3D_GetDeviceContext()->RSSetState(g_pRasterizerState);
}

void MeshField_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pRasterizerState);
}

void MeshField_Draw()
{
	Shader3d_Begin();

	Shader3d_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	Direct3D_GetDeviceContext()->OMSetDepthStencilState(g_pDepthStencilState, 0);
	Direct3D_GetDeviceContext()->RSSetState(g_pRasterizerState);

	Shader3d_SetWorldMatrix(XMMatrixIdentity());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Direct3D_GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	Direct3D_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Direct3D_GetDeviceContext()->DrawIndexed(g_NumIndex, 0, 0);
}
