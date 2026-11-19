/*============================================================================
Contents   :  [shader_manager.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/19
-----------------------------------------------------------------------------

============================================================================*/
#include "shader_manager.h"
#include "shader_vertex.h"
#include "shader_pixel.h"

// ============================================================================
// Layouts
// ============================================================================

// cube.cpp, mesh_field.cpp, model.cpp
static const D3D11_INPUT_ELEMENT_DESC LAYOUT_3D[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// grid.cpp, collision3d_debug.cpp
static const D3D11_INPUT_ELEMENT_DESC LAYOUT_POS[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// sprite.cpp, billboard.cpp (position + uv)
static const D3D11_INPUT_ELEMENT_DESC LAYOUT_POS_UV[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

struct ShaderInfo
{
	const char* vs_file;
	const char* ps_file;
	const D3D11_INPUT_ELEMENT_DESC* layout;
	int layout_count;
};

static const ShaderInfo SHADER_TABLE[] =
{
	{ "assets/shaders/shader_vertex_3d.cso",          "assets/shaders/shader_pixel_3d.cso",          LAYOUT_3D,     ARRAYSIZE(LAYOUT_3D)     }, // kShader_3D
	{ "assets/shaders/shader_vertex_primitive3D.cso", "assets/shaders/shader_pixel_primitive3D.cso", LAYOUT_POS,    ARRAYSIZE(LAYOUT_POS)    }, // kShader_Primitive3D
	{ "assets/shaders/shader_vertex_billboard.cso",   "assets/shaders/shader_pixel_billboard.cso",   LAYOUT_POS_UV, ARRAYSIZE(LAYOUT_POS_UV) }, // kShader_Billboard
	{ "assets/shaders/shader_vertex_2d.cso",          "assets/shaders/shader_pixel_2d.cso",          LAYOUT_POS_UV, ARRAYSIZE(LAYOUT_POS_UV) }, // kShader_2D
};

// Compile error for when add an enum entry and forget the table row
static_assert(ARRAYSIZE(SHADER_TABLE) == kShader_Max, "SHADER_TABLE must have one row per ShaderKind");

static VertexShader g_VertexShaders[kShader_Max];
static PixelShader  g_PixelShaders[kShader_Max];

bool ShaderManager_Initialize()
{
	for (int i = 0; i < kShader_Max; i++)
	{
		const ShaderInfo& info = SHADER_TABLE[i];

		if (!g_VertexShaders[i].Load(info.vs_file, info.layout, info.layout_count)) { return false; }
		if (!g_PixelShaders[i].Load(info.ps_file)) { return false; }
	}
	return true;
}

void ShaderManager_Finalize()
{
	for (int i = 0; i < kShader_Max; i++)
	{
		g_PixelShaders[i].Release();
		g_VertexShaders[i].Release();
	}
}

void ShaderManager_Bind(ShaderKind kind)
{
	if (kind < 0 || kind >= kShader_Max) { return; }

	g_VertexShaders[kind].Bind();
	g_PixelShaders[kind].Bind();
}
