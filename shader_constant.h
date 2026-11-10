/*============================================================================
Contents   : [shader_constant.h]

Author     : Chin Qing You
LastUpdate : 2026/11/09
-----------------------------------------------------------------------------

============================================================================*/
#ifndef SHADER_CONSTANT_H
#define SHADER_CONSTANT_H

#include <DirectXMath.h>
#include <d3d11.h>

class ShaderConstant
{
public:
	void Initialize();
	void Finalize();

	void SetWorldMatrix(const DirectX::XMMATRIX& matrix);
	void SetViewMatrix(const DirectX::XMMATRIX& matrix);
	void SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
	void SetColor(const DirectX::XMFLOAT4& color);

	void Bind();

private:
	static constexpr unsigned int VS_CONSTANT_BUFFER_COUNT = 3;
	static constexpr unsigned int PS_CONSTANT_BUFFER_COUNT = 1;

	ID3D11Buffer* m_pVSConstantBuffer[VS_CONSTANT_BUFFER_COUNT]{};
	ID3D11Buffer* m_pPSConstantBuffer[PS_CONSTANT_BUFFER_COUNT]{};

};

ShaderConstant& ShaderConstant_Get3D();

#endif
