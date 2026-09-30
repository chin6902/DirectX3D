/*============================================================================
Contents   :  [cube.cpp]

Author     : Chin Qing You
LastUpdate : 2026/09/14
-----------------------------------------------------------------------------

============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>

#include "cube.h"
#include "config.h"
#include "direct3d.h"
#include "shader3d.h"
#include "debug_ostream.h"
#include "input_keyboard.h"
#include "texture.h"

using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer{ nullptr };
static ID3D11RasterizerState* g_pRasterizerState{ nullptr };
static ID3D11DepthStencilState* g_pDepthStencilState{ nullptr };

static XMFLOAT3 g_Target{ 0.0f, 0.0f, 0.0f };
static float    g_Yaw = 0.6f;
static float    g_Pitch = 0.4f;
static float    g_Distance = 7.5f;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 uv;
};

static constexpr int NUM_VERTEX{ 36 }; //36

static int g_TextureID_Cube{ -1 };
static int g_TextureID_Stone{ -1 };

void Cube_Initialize()
{
	g_TextureID_Cube = Texture_Load(L"assets/textures/cube.png", true);

	D3D11_BUFFER_DESC bd{
		.ByteWidth = sizeof(Vertex) * NUM_VERTEX,
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};
	
	constexpr float u0{ 1.0f / 3.0f };
	constexpr float v0{ 1.0f / 2.0f };
	constexpr float u1{ u0 * 2.0f };
	constexpr float u2{ 1.0f };
	constexpr float v1{ 1.0f };

	Vertex v[NUM_VERTEX]{
		// Front face  (col 1, row 0)
		{ { -0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v0  } },
		{ { -0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, 0.0f} },
		{ {  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, 0.0f} },
		{ { -0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v0  } },
		{ {  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, 0.0f} },
		{ {  0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0  } },

		// Right face  (col 2, row 0)
		{ { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0  } },
		{ { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, 0.0f} },
		{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, 0.0f} },
		{ { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0  } },
		{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, 0.0f} },
		{ { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v0  } },

		// Left face  (col 0, row 1)
		{ { -0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v1} },
		{ { -0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v0} },
		{ { -0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v0} },
		{ { -0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v1} },
		{ { -0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v0} },
		{ { -0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v1} },

		// Top face  (col 0, row 0)
		{ { -0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v0  } },
		{ { -0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
		{ {  0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   0.0f} },
		{ { -0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v0  } },
		{ {  0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   0.0f} },
		{ {  0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v0  } },

		// Back face  (col 1, row 1)
		{ {  0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v1} },
		{ {  0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v0} },
		{ { -0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0} },
		{ {  0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v1} },
		{ { -0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0} },
		{ { -0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v1} },

		// Bottom face  (col 2, row 1)
		{ { -0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v1} },
		{ { -0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0} },
		{ {  0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v0} },
		{ { -0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v1} },
		{ {  0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v0} },
		{ {  0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v1} }
	};

	D3D11_SUBRESOURCE_DATA sd{
		.pSysMem = v
	};

	HRESULT hr = Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

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

void Cube_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pRasterizerState);
	Texture_Release(g_TextureID_Cube);
}

void Cube_Update(float delta_time)
{
}

void Cube_Draw(const XMMATRIX& world)
{
	Shader3d_Begin();
	Texture_SetTexture(g_TextureID_Cube);

	Direct3D_GetDeviceContext()->OMSetDepthStencilState(g_pDepthStencilState, 0);
	Direct3D_GetDeviceContext()->RSSetState(g_pRasterizerState);

	// practice (scale * rotation * translation)
	// XMMATRIX world = XMMatrixScaling(s, s, s) * XMMatrixRotationY(g_Angle) * XMMatrixTranslation(x, y, z);

	Shader3d_SetWorldMatrix(world);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Direct3D_GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Direct3D_GetDeviceContext()->Draw(NUM_VERTEX, 0);
}
