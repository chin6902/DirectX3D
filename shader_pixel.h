/*============================================================================
Contents   : [shader_pixel.h]

Author     : Chin Qing You
LastUpdate : 2026/11/09
-----------------------------------------------------------------------------
// make a manager for pixel shader(for even better organization)
============================================================================*/
#ifndef SHADER_PIXEL_H
#define SHADER_PIXEL_H

#include <d3d11.h>

class PixelShader
{
public:
	PixelShader() = default;
	~PixelShader();

	bool Load(const char* filename);
	void Begin();

private:
	ID3D11PixelShader* g_pPixelShader = nullptr;

};

#endif
