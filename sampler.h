/*============================================================================
Contents   :  [sampler.h]

Author     : Chin Qing You
LastUpdate : 2026/09/28
-----------------------------------------------------------------------------

============================================================================*/
#ifndef SAMPLER_H
#define SAMPLER_H

enum SamplerFilter
{
	kSamplerFilter_Point,
	kSamplerFilter_Linear,
	kSamplerFilter_Anisotropic,
	kSamplerFilter_Max,
};

enum SamplerAddress
{
	kSamplerAddress_Wrap,
	kSamplerAddress_Clamp,
	kSamplerAddress_Mirror,
	kSamplerAddress_Border,
	kSamplerAddress_Max,
};

bool Sampler_Initialize();
void Sampler_Finalize();

// Binds the sampler to slot s0 of the pixel shader
void Sampler_SetFilter(SamplerFilter filter, SamplerAddress address = kSamplerAddress_Wrap);

#endif