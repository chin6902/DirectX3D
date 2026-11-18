#include "billboard.h"
#include "cube.h"
#include "config.h"
#include "direct3d.h"
#include "debug_ostream.h"
#include "texture.h"
#include "shader_constant.h"

using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer{ nullptr };
// index buffer
static ID3D11Buffer* g_pIndexBuffer{ nullptr };

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
};

static constexpr int NUM_VERTEX{ 4 };

static ShaderConstant* g_pShaderConstant{ nullptr };

void Billboard_Initialize(ShaderConstant* pShaderConstant)
{
	g_pShaderConstant = pShaderConstant;

	D3D11_BUFFER_DESC bd{
		.ByteWidth = sizeof(Vertex) * NUM_VERTEX,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0
	};

	Vertex v[NUM_VERTEX]{
		// Front face  (col 1, row 0)
		{ { -0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },  
		{ { -0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} },  
		{ {  0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },  
		{ {  0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },  
	};

	D3D11_SUBRESOURCE_DATA sd{
	.pSysMem = v
	};

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Billboard_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}

void Billboard_SetViewMatrix(const DirectX::XMMATRIX& view)
{

}

void Billboard_Draw(const DirectX::XMMATRIX& world)
{
	ShaderConstant_Get3D().SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	ShaderConstant_Get3D().SetWorldMatrix(world);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Direct3D_GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// unsinged int -> DXGI_FORMAT_R32_UINT

	Direct3D_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Direct3D_GetDeviceContext()->Draw(NUM_VERTEX, 0);
}
