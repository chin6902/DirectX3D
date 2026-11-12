/*============================================================================
Contents   : [shader_vertex.h]

Author     : Chin Qing You
LastUpdate : 2026/11/09
-----------------------------------------------------------------------------
// make a manager for vertex shader(for even better organization)
============================================================================*/
#ifndef SHADER_VERTEX_H
#define SHADER_VERTEX_H

#include <d3d11.h>

class VertexShader
{
public:
    VertexShader() = default;
    ~VertexShader();
    VertexShader(const VertexShader&) = delete;
    VertexShader& operator=(const VertexShader&) = delete;

    bool Load(const char* filename, const D3D11_INPUT_ELEMENT_DESC* layout, int num_elements);
    void Bind();
    void Release();

private:
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;

};

#endif
