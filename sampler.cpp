/*============================================================================
Contents   :  [sampler.cpp]

Author     : Chin Qing You
LastUpdate : 2026/09/28
-----------------------------------------------------------------------------

============================================================================*/
#include "sampler.h"
#include "direct3d.h"
#include "debug_ostream.h"

using namespace std;

static ID3D11SamplerState* g_pSamplerStates[kSamplerFilter_Max][kSamplerAddress_Max]{};

static constexpr D3D11_FILTER FILTER_TABLE[kSamplerFilter_Max] =
{
	D3D11_FILTER_MIN_MAG_MIP_POINT,
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	D3D11_FILTER_ANISOTROPIC,
};

static constexpr D3D11_TEXTURE_ADDRESS_MODE ADDRESS_TABLE[kSamplerAddress_Max] =
{
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_MIRROR,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_BORDER,
};

static constexpr UINT ANISOTROPY_MAX = 8;

bool Sampler_Initialize()
{
	for (int f = 0; f < kSamplerFilter_Max; f++)
	{
		for (int a = 0; a < kSamplerAddress_Max; a++)
		{
			D3D11_SAMPLER_DESC sd{};
			sd.Filter = FILTER_TABLE[f];
			sd.AddressU = ADDRESS_TABLE[a];
			sd.AddressV = ADDRESS_TABLE[a];
			sd.AddressW = ADDRESS_TABLE[a];
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sd.MinLOD = 0.0f;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			if (f == kSamplerFilter_Anisotropic)
			{
				sd.MaxAnisotropy = ANISOTROPY_MAX;
			}
			HRESULT hr = Direct3D_GetDevice()->CreateSamplerState(&sd, &g_pSamplerStates[f][a]);
			if (FAILED(hr))
			{
				hal::dout << "Sampler_Initialize(): sampler creation failed (filter " << f << ", address " << a << ")" << endl;				
				return false;
			}
		}
	}
	return true;
}

void Sampler_Finalize()
{
	for (auto& row : g_pSamplerStates)
	{
		for (ID3D11SamplerState*& sampler : row)
		{
			SAFE_RELEASE(sampler);
		}
	}
}

void Sampler_SetFilter(SamplerFilter filter, SamplerAddress address)
{
	if (filter < 0 || filter >= kSamplerFilter_Max) { return; }
	if (address < 0 || address >= kSamplerAddress_Max) { return; }

	Direct3D_GetDeviceContext()->PSSetSamplers(0, 1, &g_pSamplerStates[filter][address]);
}