/*============================================================================
Contents   :  [render_state.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/17
-----------------------------------------------------------------------------

============================================================================*/
#include "render_state.h"
#include "direct3d.h"
#include "debug_ostream.h"

static ID3D11DepthStencilState* g_pDepthStates[kDepth_Max]{};
static ID3D11BlendState* g_pBlendStates[kBlend_Max]{};
static ID3D11RasterizerState* g_pRasterizerStates[kCull_Max]{};

static constexpr D3D11_CULL_MODE CULL_TABLE[kCull_Max] =
{
	D3D11_CULL_NONE,   
	D3D11_CULL_BACK,   
	D3D11_CULL_FRONT, 
};

bool RenderState_Initialize()
{
	ID3D11Device* device = Direct3D_GetDevice();

	// --- Depth ---
	for (int i = 0; i < kDepth_Max; i++)
	{
		D3D11_DEPTH_STENCIL_DESC dsd{};
		dsd.DepthEnable = (i != kDepth_Off);
		dsd.DepthWriteMask = (i == kDepth_On) ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		dsd.DepthFunc = D3D11_COMPARISON_LESS;

		if (FAILED(device->CreateDepthStencilState(&dsd, &g_pDepthStates[i])))
		{
			hal::dout << "RenderState_Initialize(): depth state creation failed (" << i << ")" << std::endl;
			return false;
		}
	}

	// --- Blend ---
	for (int i = 0; i < kBlend_Max; i++)
	{
		D3D11_BLEND_DESC bd{};
		D3D11_RENDER_TARGET_BLEND_DESC& rt = bd.RenderTarget[0];

		rt.BlendEnable = (i != kBlend_Opaque);
		rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend = (i == kBlend_Additive) ? D3D11_BLEND_ONE : D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOp = D3D11_BLEND_OP_ADD;
		rt.SrcBlendAlpha = D3D11_BLEND_ONE;
		rt.DestBlendAlpha = D3D11_BLEND_ZERO;
		rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if (FAILED(device->CreateBlendState(&bd, &g_pBlendStates[i])))
		{
			hal::dout << "RenderState_Initialize(): blend state creation failed (" << i << ")" << std::endl;
			return false;
		}
	}

	// --- Rasterizer ---
	for (int i = 0; i < kCull_Max; i++)
	{
		D3D11_RASTERIZER_DESC rd{};
		rd.FillMode = D3D11_FILL_SOLID;   
		rd.CullMode = CULL_TABLE[i];
		rd.DepthClipEnable = TRUE;         

		if (FAILED(device->CreateRasterizerState(&rd, &g_pRasterizerStates[i])))
		{
			hal::dout << "RenderState_Initialize(): rasterizer state creation failed (" << i << ")" << std::endl;
			return false;
		}
	}

	return true;
}

void RenderState_Finalize()
{
	for (auto& state : g_pRasterizerStates) { SAFE_RELEASE(state); }
	for (auto& state : g_pBlendStates) { SAFE_RELEASE(state); }
	for (auto& state : g_pDepthStates) { SAFE_RELEASE(state); }
}

void RenderState_SetDepth(DepthMode mode)
{
	if (mode < 0 || mode >= kDepth_Max) { return; }
	Direct3D_GetDeviceContext()->OMSetDepthStencilState(g_pDepthStates[mode], 0);
}

void RenderState_SetBlend(BlendMode mode)
{
	if (mode < 0 || mode >= kBlend_Max) { return; }
	Direct3D_GetDeviceContext()->OMSetBlendState(g_pBlendStates[mode], nullptr, 0xFFFFFFFF);
}

void RenderState_SetCull(CullMode mode)
{
	if (mode < 0 || mode >= kCull_Max) { return; }
	Direct3D_GetDeviceContext()->RSSetState(g_pRasterizerStates[mode]);
}

void RenderState_Set3D()
{
	RenderState_SetDepth(kDepth_On);
	RenderState_SetCull(kCull_Back);
	RenderState_SetBlend(kBlend_Opaque);
}

void RenderState_Set2D()
{
	RenderState_SetDepth(kDepth_Off);
	RenderState_SetCull(kCull_None);
	RenderState_SetBlend(kBlend_Alpha);
}