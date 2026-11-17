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
	ShaderConstant() = default;
	~ShaderConstant();
	ShaderConstant(const ShaderConstant&) = delete;
	ShaderConstant& operator=(const ShaderConstant&) = delete;
	void Initialize();
	void Finalize();

	void SetWorldMatrix(const DirectX::XMMATRIX& matrix) const;
	void SetViewMatrix(const DirectX::XMMATRIX& matrix) const;
	void SetProjectionMatrix(const DirectX::XMMATRIX& matrix) const;
	void SetColor(const DirectX::XMFLOAT4& color);
	void SetUVMatrix(const DirectX::XMMATRIX& matrix) const;

	void Bind() const;

private:
	// 0 -> world + worldIT 
	// 1 -> view
	// 2 -> projection
	// 3 -> uv matrix
	static constexpr unsigned int VS_CONSTANT_BUFFER_COUNT = 4;

	// 0 -> light
	// 1 -> color
	static constexpr unsigned int PS_CONSTANT_BUFFER_COUNT = 1;

	ID3D11Buffer* m_pVSConstantBuffer[VS_CONSTANT_BUFFER_COUNT]{};
	ID3D11Buffer* m_pPSConstantBuffer[PS_CONSTANT_BUFFER_COUNT]{};

};

ShaderConstant& ShaderConstant_Get3D();
ShaderConstant& ShaderConstant_Get2D();

#endif
