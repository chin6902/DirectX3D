/*============================================================================
Contents   :  [billboard.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/17
-----------------------------------------------------------------------------
set offset function for root as base or center  of the billboard
============================================================================*/
#include <algorithm>

#include "billboard.h"
#include "direct3d.h"
#include "texture.h"
#include "sampler.h"
#include "shader_constant.h"
#include "render_state.h"
#include "shader_manager.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
};

struct BillboardEntry
{
	int texture_id;
	XMFLOAT3 position;
	BillboardDrawParams params;
	float depth;      // how far in front of the camera, filled in at flush
};

static constexpr int NUM_VERTEX{ 4 };

static ID3D11Buffer* g_pVertexBuffer{ nullptr };

static constexpr int BILLBOARD_MAX{ 256 };
static BillboardEntry g_Entries[BILLBOARD_MAX];
static int g_EntryCount{ 0 };

static XMFLOAT4X4 g_View{};

void Billboard_Initialize()
{
	// Centered on the origin (z = 0), zig-zag order for a triangle strip
	Vertex v[NUM_VERTEX]{
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },  // bottom-left
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },  // top-left
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },  // bottom-right
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } },  // top-right
	};

	D3D11_BUFFER_DESC bd{
		.ByteWidth = sizeof(Vertex) * NUM_VERTEX,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
	};
	D3D11_SUBRESOURCE_DATA sd{ .pSysMem = v };

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	g_EntryCount = 0;

	XMStoreFloat4x4(&g_View, XMMatrixIdentity());
}

void Billboard_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void Billboard_SetViewMatrix(const XMMATRIX& view)
{
	XMStoreFloat4x4(&g_View, view);
}

void Billboard_Draw(int texture_id, const XMFLOAT3& position)
{
	Billboard_Draw(texture_id, position, BillboardDrawParams{});
}

void Billboard_Draw(int texture_id, const XMFLOAT3& position, const XMFLOAT2& size)
{
	Billboard_Draw(texture_id, position, BillboardDrawParams{ .size = size });
}

void Billboard_Draw(int texture_id, const XMFLOAT3& position, const BillboardDrawParams& params)
{
	if (g_EntryCount >= BILLBOARD_MAX) { return; }
	g_Entries[g_EntryCount++] = { texture_id, position, params, 0.0f };
}

void Billboard_Flush()
{
	if (g_EntryCount == 0) { return; }

	const XMMATRIX view = XMLoadFloat4x4(&g_View);

	// 1. depth = z in view space (how far in front of the camera)
	for (int i = 0; i < g_EntryCount; i++)
	{
		XMVECTOR pos_view = XMVector3TransformCoord(XMLoadFloat3(&g_Entries[i].position), view);
		g_Entries[i].depth = XMVectorGetZ(pos_view);
	}

	// 2. sort far -> near
	std::sort(g_Entries, g_Entries + g_EntryCount, [](const BillboardEntry& a, const BillboardEntry& b) { return a.depth > b.depth; });

	// 3. rotation that faces the camera (transpose of the view rotation)
	XMMATRIX face = view;
	face.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	face = XMMatrixTranspose(face);

	// 4. states and vertex buffer, once for all billboards
	RenderState_SetDepth(kDepth_ReadOnly);
	RenderState_SetBlend(kBlend_Alpha);
	RenderState_SetCull(kCull_None);
	Sampler_SetFilter(kSamplerFilter_Linear, kSamplerAddress_Clamp);
	ShaderManager_Bind(kShader_Billboard);              // ← was kShader_3D
	ShaderConstant_Get3D().SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	ShaderConstant_Get3D().Bind();

	ID3D11DeviceContext* ctx = Direct3D_GetDeviceContext();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 5. draw each one, far first
	for (int i = 0; i < g_EntryCount; i++)
	{
		const BillboardEntry& e = g_Entries[i];
		const BillboardDrawParams& p = e.params;

		const float offset_x = 0.5f - p.pivot.x;
		const float offset_y = p.pivot.y - 0.5f;

		XMMATRIX world =
			XMMatrixTranslation(offset_x, offset_y, 0.0f) *				   // 1. pivot
			XMMatrixScaling(p.size.x, p.size.y, 1.0f) *                    // 2. size
			face *                                                         // 3. face camera
			XMMatrixTranslation(e.position.x, e.position.y, e.position.z); // 4. place

		XMMATRIX uv =
			XMMatrixScaling(p.uv_rect.z, p.uv_rect.w, 1.0f) *
			XMMatrixTranslation(p.uv_rect.x, p.uv_rect.y, 0.0f);

		ShaderConstant_Get3D().SetWorldMatrix(world);
		ShaderConstant_Get3D().SetUVMatrix(uv);
		Texture_SetTexture(e.texture_id);

		ctx->Draw(NUM_VERTEX, 0);
	}

	ShaderConstant_Get3D().SetUVMatrix(XMMatrixIdentity());
	
	g_EntryCount = 0;
}