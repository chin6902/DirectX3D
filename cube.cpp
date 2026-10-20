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
#include "texture.h"

using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer{ nullptr };
// index buffer
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

static constexpr int NUM_VERTEX{ 4 * 6 }; 
static constexpr int NUM_INDEX { 6 * 6 };

static int g_TextureID_Cube{ -1 };
static int g_TextureID_Stone{ -1 };

void Cube_Initialize()
{
	g_TextureID_Cube = Texture_Load(L"assets/textures/cube.png", true);

	D3D11_BUFFER_DESC bd{
		.ByteWidth = sizeof(Vertex) * NUM_VERTEX,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0
	};
	
	constexpr float u0{ 1.0f / 3.0f };
	constexpr float v0{ 1.0f / 2.0f };
	constexpr float u1{ u0 * 2.0f };
	constexpr float u2{ 1.0f };
	constexpr float v1{ 1.0f };

	Vertex v[NUM_VERTEX]{
		// Front face  (col 1, row 0)
		{ { -0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v0  } },  //0  // 012(first triangle) -> 023(second triangle)
		{ { -0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, 0.0f} },  //1
		{ {  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, 0.0f} },  //2
		{ {  0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0  } },	//3

		// Right face  (col 2, row 0)
		{ { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0  } },
		{ { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, 0.0f} },
		{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, 0.0f} },
		{ { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v0  } },

		// Left face  (col 0, row 1)
		{ { -0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v1} },
		{ { -0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v0} },
		{ { -0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v0} },
		{ { -0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v1} },

		// Top face  (col 0, row 0)
		{ { -0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, v0  } },
		{ { -0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
		{ {  0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   0.0f} },
		{ {  0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0,   v0  } },

		// Back face  (col 1, row 1)
		{ {  0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v1} },
		{ {  0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u0, v0} },
		{ { -0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0} },
		{ { -0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v1} },

		// Bottom face  (col 2, row 1)
		{ { -0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v1} },
		{ { -0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u1, v0} },
		{ {  0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v0} },
		{ {  0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {u2, v1} }
	};

	D3D11_SUBRESOURCE_DATA sd{
	.pSysMem = v
	};

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	// Create index buffer
	D3D11_BUFFER_DESC ibd{
		.ByteWidth = sizeof(unsigned short) * NUM_INDEX,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0
	};

	// Create index data
	unsigned short index[NUM_INDEX]{
		0, 1, 2, 0, 2, 3,       // Front face
		4, 5, 6, 4, 6, 7,       // Right face
		8, 9,10, 8,10,11,       // Left face
	   12,13,14,12,14,15,       // Top face
	   16,17,18,16,18,19,       // Back face
	   20,21,22,20,22,23        // Bottom face
	};

	sd.pSysMem = index;
	Direct3D_GetDevice()->CreateBuffer(&ibd, &sd, &g_pIndexBuffer);

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
	SAFE_RELEASE(g_pIndexBuffer);
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

	Direct3D_GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// unsinged int -> DXGI_FORMAT_R32_UINT

	Direct3D_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Direct3D_GetDeviceContext()->Draw(NUM_VERTEX, 0);
	Direct3D_GetDeviceContext()->DrawIndexed(NUM_INDEX, 0, 0);
}
