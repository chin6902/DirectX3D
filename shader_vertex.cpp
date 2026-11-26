/*============================================================================
Contents   : [shader_vertex.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/09
-----------------------------------------------------------------------------

============================================================================*/
#include "shader_vertex.h"
#include "direct3d.h"
#include "debug_ostream.h"
#include <fstream>
#include <string>

VertexShader::~VertexShader() 
{
	Release();
}

void VertexShader::Release()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
}

bool VertexShader::Load(const char* filename, const D3D11_INPUT_ELEMENT_DESC * layout, int num_elements)
{
	Release();

	HRESULT hr; // 戻り値格納用

	// 事前コンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs(filename, std::ios::binary);

	if (!ifs_vs) {
		const std::string errorMessage = "Vertex shader file load error\n\n" + std::string(filename);
		MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_OK);
		return false;
	}

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナリデータを格納するためのバッファを確保
	unsigned char* vsbinary_pointer = new unsigned char[filesize];

	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	hr = Direct3D_GetDevice()->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		delete[] vsbinary_pointer;
		hal::dout << "VertexShader::Load : 頂点シェーダーの作成に失敗しました(" << filename << ")" << std::endl;
		return false;
	}

	// 頂点レイアウトの作成
	hr = Direct3D_GetDevice()->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &m_pInputLayout);

	delete[] vsbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr))
	{
		hal::dout << "VertexShader::Load : 頂点レイアウトの作成に失敗しました(" << filename << ")" << std::endl;
		return false;
	}

	return true;
}

void VertexShader::Bind()
{
	Direct3D_GetDeviceContext()->VSSetShader(m_pVertexShader, nullptr, 0);

	Direct3D_GetDeviceContext()->IASetInputLayout(m_pInputLayout);
}
