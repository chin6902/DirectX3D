/*============================================================================
Contents   :  [grid.cpp]

Author     : Chin Qing You
LastUpdate : 2026/09/16
-----------------------------------------------------------------------------

============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>

#include "grid.h"
#include "config.h"
#include "direct3d.h"
#include "shader3d.h"
#include "debug_ostream.h"
#include "texture.h"

using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer{ nullptr };

static constexpr int GRID_COUNT_X{ 10 };
static constexpr int GRID_COUNT_Z{ 10 };
static constexpr int GRID_LINE_COUNT_X{ GRID_COUNT_X + 1 };
static constexpr int GRID_LINE_COUNT_Z{ GRID_COUNT_Z + 1 };
static constexpr int NUM_VERTEX{ GRID_LINE_COUNT_X * 2 + GRID_LINE_COUNT_Z * 2 };

static int g_TextureID_Grid{ -1 };

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 uv;		// dummy, not used in this grid
};


void Grid_Initialize()
{
	g_TextureID_Grid = Texture_Load(L"assets/textures/white.png", true);

	D3D11_BUFFER_DESC bd{
	.ByteWidth = sizeof(Vertex) * NUM_VERTEX,
	.Usage = D3D11_USAGE_DYNAMIC,
	.BindFlags = D3D11_BIND_VERTEX_BUFFER,
	.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};

	Vertex v[NUM_VERTEX]{};

	constexpr float GRID_SIZE_X{ 1.0f * GRID_COUNT_X };
	constexpr float GRID_SIZE_Z{ 1.0f * GRID_COUNT_Z };
	constexpr float START_X { GRID_SIZE_X * -0.5f };
	constexpr float START_Z { GRID_SIZE_Z * -0.5f };
	int index = 0;
	for (int i = 0; i < GRID_LINE_COUNT_X; ++i)
	{
		float x = START_X + i * 1.0f;	
		v[index++] = { {x, 0.0f, START_Z              }, {0.5f, 1.0f, 0.0f, 1.0f} };
		v[index++] = { {x, 0.0f, START_Z + GRID_SIZE_Z}, {0.5f, 1.0f, 0.0f, 1.0f} };
	}

	for (int i = 0; i < GRID_LINE_COUNT_Z; ++i)	
	{
		float z = START_Z + i * 1.0f;
		v[index + 0] = { {START_X              , 0.0f, z}, {0.5f, 1.0f, 0.0f, 1.0f} };
		v[index + 1] = { {START_X + GRID_SIZE_X, 0.0f, z}, {0.5f, 1.0f, 0.0f, 1.0f} };
		index+=2;
	}

	D3D11_SUBRESOURCE_DATA sd{
		.pSysMem = v
	};

	HRESULT hr = Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Grid_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);

	Texture_Release(g_TextureID_Grid);
}

void Grid_Update(float delta_time)
{

}

void Grid_Draw()
{
	Shader3d_Begin();

	Texture_SetTexture(g_TextureID_Grid);

	Shader3d_SetWorldMatrix(XMMatrixIdentity());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Direct3D_GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Direct3D_GetDeviceContext()->Draw(NUM_VERTEX, 0);
}
