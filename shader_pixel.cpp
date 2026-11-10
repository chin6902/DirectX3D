/*============================================================================
Contents   : [shader_pixel.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/09
-----------------------------------------------------------------------------

============================================================================*/
#include "shader_pixel.h"
#include "direct3d.h"
#include "debug_ostream.h"
#include <fstream>
#include <string>

PixelShader::~PixelShader()
{
	Release();
}

void PixelShader::Release()
{
	SAFE_RELEASE(m_pPixelShader);
}

bool PixelShader::Load(const char* filename)
{
	Release();

	HRESULT hr; // 戻り値格納用

	// 事前コンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs(filename, std::ios::binary);

	if (!ifs_vs) {
		const std::string errorMessage = "ピクセルシェーダーの読み込みに失敗しました\n\n" + std::string(filename);
		MessageBoxA(nullptr, errorMessage.c_str(), "エラー", MB_OK);
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

	// ピクセルシェーダーの作成
	hr = Direct3D_GetDevice()->CreatePixelShader(vsbinary_pointer, filesize, nullptr, &m_pPixelShader);

	delete[] vsbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr))
	{
		hal::dout << "PixelShader::Load : ピクセルシェーダーの作成に失敗しました(" << filename << ")" << std::endl;
		return false;
	}

	return true;
}

void PixelShader::Begin()
{
	Direct3D_GetDeviceContext()->PSSetShader(m_pPixelShader, nullptr, 0);
}
