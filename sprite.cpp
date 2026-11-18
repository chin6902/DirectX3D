/*============================================================================
Contents   :  [sprite.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/17
-----------------------------------------------------------------------------

============================================================================*/
#include <algorithm>
#include <cmath>

#include "sprite.h"
#include "debug_ostream.h"
#include "config.h"
#include "direct3d.h"
#include "shader_vertex.h"
#include "shader_pixel.h"
#include "shader_constant.h"
#include "texture.h"
#include "sampler.h"
#include "render_state.h"

using namespace DirectX;

// ============================================================================
// Vertex
// ============================================================================
// Must match layout2d below (POSITION + TEXCOORD, 20 bytes)
struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
};

static constexpr int BATCH_MAX_QUADS = 4096;
static constexpr int VERTS_PER_QUAD = 6;
static constexpr int BATCH_MAX_VERTICES = BATCH_MAX_QUADS * VERTS_PER_QUAD;

static Vertex g_Vertices[BATCH_MAX_VERTICES];
static int    g_VertexCount = 0;

static int g_BatchTexture = TEXTURE_INVALID_ID;
static SpriteFilter g_BatchFilter = kSpriteFilter_Point;
static SpriteFilter g_PendingFilter = kSpriteFilter_Point;
static XMFLOAT4     g_BatchColor{ 1.0f, 1.0f, 1.0f, 1.0f }; // color is a shader constant now, so one color per batch

static VertexShader g_VertexShader2D;
static PixelShader  g_PixelShader2D;

static ID3D11Buffer* g_pVertexBuffer{ nullptr };

// ============================================================================
// Initialize
// ============================================================================
bool Sprite_Initialize()
{
	// --- 2D shaders ---
	D3D11_INPUT_ELEMENT_DESC layout2d[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!g_VertexShader2D.Load("assets/shaders/shader_vertex_2d.cso", layout2d, ARRAYSIZE(layout2d))) { return false; }
	if (!g_PixelShader2D.Load("assets/shaders/shader_pixel_2d.cso")) { return false; }

	// --- 2D constants ---
	// The screen size doesn't change, so the projection is written once here instead of every flush
	ShaderConstant_Get2D().Initialize();
	ShaderConstant_Get2D().SetProjectionMatrix(
		XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	// --- DYNAMIC vertex buffer, rewritten every frame ---
	{
		D3D11_BUFFER_DESC bd{
			.ByteWidth = sizeof(Vertex) * BATCH_MAX_VERTICES,
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
		};

		if (FAILED(Direct3D_GetDevice()->CreateBuffer(&bd, nullptr, &g_pVertexBuffer)))
		{
			hal::dout << "Sprite_Initialize(): 頂点バッファの作成に失敗しました" << std::endl;
			return false;
		}
	}

	g_VertexCount = 0;
	g_BatchTexture = TEXTURE_INVALID_ID;
	g_BatchFilter = kSpriteFilter_Point;
	g_PendingFilter = kSpriteFilter_Point;
	g_BatchColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	return true;
}

void Sprite_Finalize()
{
	Texture_AllRelease();
	SAFE_RELEASE(g_pVertexBuffer);

	ShaderConstant_Get2D().Finalize();
	g_PixelShader2D.Release();
	g_VertexShader2D.Release();
}

// ============================================================================
// Flush
// ============================================================================
void Sprite_Flush()
{
	if (g_VertexCount == 0 || g_BatchTexture == TEXTURE_INVALID_ID)
	{
		g_VertexCount = 0;
		return;
	}

	ID3D11DeviceContext* ctx = Direct3D_GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mapped{};
	if (SUCCEEDED(ctx->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		memcpy(mapped.pData, g_Vertices, sizeof(Vertex) * g_VertexCount);
		ctx->Unmap(g_pVertexBuffer, 0);
	}

	// The 3D pass binds its own shaders and constant buffers, so rebind ours every flush
	g_VertexShader2D.Bind();
	g_PixelShader2D.Bind();
	ShaderConstant_Get2D().SetColor(g_BatchColor);
	ShaderConstant_Get2D().Bind();

	Sampler_SetFilter((g_BatchFilter == kSpriteFilter_Linear) ? kSamplerFilter_Linear : kSamplerFilter_Point, kSamplerAddress_Clamp);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Texture_SetTexture(g_BatchTexture);

	RenderState_Set2D();

	ctx->Draw(g_VertexCount, 0);

	g_VertexCount = 0;
}

void Sprite_SetFilter(SpriteFilter filter)
{
	g_PendingFilter = filter;
}

// ============================================================================
// Internal draw
// ============================================================================
static void Sprite_DrawInternal(
	int   texture_id,
	float x, float y,
	float width, float height,
	float texture_x, float texture_y,
	int   texture_width, int texture_height,
	const SpriteDrawParams& params)
{
	if (texture_id == TEXTURE_INVALID_ID) { return; }

	const unsigned int tex_w = Texture_GetWidth(texture_id);
	const unsigned int tex_h = Texture_GetHeight(texture_id);
	if (tex_w == 0 || tex_h == 0) { return; }

	const XMFLOAT4 rgba{ params.color.x, params.color.y, params.color.z, params.alpha };

	const bool color_changed =
		rgba.x != g_BatchColor.x || rgba.y != g_BatchColor.y ||
		rgba.z != g_BatchColor.z || rgba.w != g_BatchColor.w;

	// --- state change ends the current batch ---
	if (texture_id != g_BatchTexture
		|| g_PendingFilter != g_BatchFilter
		|| color_changed
		|| g_VertexCount + VERTS_PER_QUAD > BATCH_MAX_VERTICES)
	{
		Sprite_Flush();
		g_BatchTexture = texture_id;
		g_BatchFilter = g_PendingFilter;
		g_BatchColor = rgba;
	}

	// --- geometry ---
	const float scaleX = params.flip_x ? -(params.scale.x) : params.scale.x;
	const float scaleY = params.flip_y ? -(params.scale.y) : params.scale.y;

	const float hw = width * scaleX * 0.5f;
	const float hh = height * scaleY * 0.5f;

	const float cx = x + width * params.scale.x * 0.5f;
	const float cy = y + height * params.scale.y * 0.5f;

	XMFLOAT2 corner[4] =
	{
		{ -hw, -hh },   // 0 top-left
		{  hw, -hh },   // 1 top-right
		{ -hw,  hh },   // 2 bottom-left
		{  hw,  hh },   // 3 bottom-right
	};

	if (params.angle != 0.0f)
	{
		const float s = sinf(params.angle);
		const float c = cosf(params.angle);

		for (XMFLOAT2& v : corner)
		{
			const float rx = v.x * c - v.y * s;
			const float ry = v.x * s + v.y * c;
			v = { rx, ry };
		}
	}

	for (XMFLOAT2& v : corner) { v = { v.x + cx, v.y + cy }; }

	// --- UVs ---
	const float tx = texture_x / static_cast<float>(tex_w);
	const float ty = texture_y / static_cast<float>(tex_h);
	const float tw = texture_width / static_cast<float>(tex_w);
	const float th = texture_height / static_cast<float>(tex_h);

	const XMFLOAT2 uv[4] =
	{
		{ tx,      ty      },
		{ tx + tw, ty      },
		{ tx,      ty + th },
		{ tx + tw, ty + th },
	};

	static constexpr int order[VERTS_PER_QUAD] = { 0, 1, 2,  1, 3, 2 };

	for (int i = 0; i < VERTS_PER_QUAD; i++)
	{
		const int cidx = order[i];
		Vertex& v = g_Vertices[g_VertexCount++];

		v.position = { corner[cidx].x, corner[cidx].y, 0.0f };
		v.texcoord = uv[cidx];
	}
}

static const SpriteDrawParams DEFAULT_PARAMS{};

// ============================================================================
// Public overloads
// ============================================================================
void Sprite_Draw(int texture_id, float x, float y)
{
	Sprite_Draw(texture_id, x, y, DEFAULT_PARAMS);
}

void Sprite_Draw(int texture_id, float x, float y, const SpriteDrawParams& params)
{
	const float w = static_cast<float>(Texture_GetWidth(texture_id));
	const float h = static_cast<float>(Texture_GetHeight(texture_id));

	Sprite_DrawInternal(texture_id, x, y, w, h, 0.0f, 0.0f,
		Texture_GetWidth(texture_id), Texture_GetHeight(texture_id), params);
}

void Sprite_Draw(int texture_id, float x, float y, float width, float height)
{
	Sprite_Draw(texture_id, x, y, width, height, DEFAULT_PARAMS);
}

void Sprite_Draw(int texture_id, float x, float y,
	float width, float height, const SpriteDrawParams& params)
{
	Sprite_DrawInternal(texture_id, x, y, width, height, 0.0f, 0.0f,
		Texture_GetWidth(texture_id), Texture_GetHeight(texture_id), params);
}

void Sprite_Draw(int texture_id, float x, float y,
	float width, float height,
	float texture_x, float texture_y,
	int texture_width, int texture_height)
{
	Sprite_Draw(texture_id, x, y, width, height,
		texture_x, texture_y, texture_width, texture_height, DEFAULT_PARAMS);
}

void Sprite_Draw(int texture_id, float x, float y,
	float width, float height,
	float texture_x, float texture_y,
	int texture_width, int texture_height,
	const SpriteDrawParams& params)
{
	Sprite_DrawInternal(texture_id, x, y, width, height,
		texture_x, texture_y, texture_width, texture_height, params);
}