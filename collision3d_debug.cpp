/*============================================================================
Contents   :  [collision3d_debug.cpp]

Author     : Chin Qing You
LastUpdate : 2026/11/10
-----------------------------------------------------------------------------

============================================================================*/
#include <DirectXMath.h>
#include <cstring>

#include "collision3d_debug.h"
#include "direct3d.h"

using namespace DirectX;

constexpr unsigned int LINE_COUNT_MAX{ 10000 };

static ID3D11Buffer* g_pVertexBuffer{ nullptr };
static ID3D11DepthStencilState* g_pDepthStateOff{ nullptr };
static XMFLOAT3 g_LineBuffer[LINE_COUNT_MAX * 2]{};
static unsigned int g_LineCount{ 0 };

void Collision3D_Debug_Initialize()
{
    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = FALSE;                         // ignore depth: always visible
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    Direct3D_GetDevice()->CreateDepthStencilState(&dsd, &g_pDepthStateOff);

	D3D11_BUFFER_DESC bd{
			.ByteWidth = sizeof(XMFLOAT3) * LINE_COUNT_MAX * 2,
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};

	Direct3D_GetDevice()->CreateBuffer(&bd, nullptr, &g_pVertexBuffer);

	g_LineCount = 0;
}

void Collision3D_Debug_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pDepthStateOff);
}

static void AddLine(const XMFLOAT3& a, const XMFLOAT3& b)
{
    if (g_LineCount >= LINE_COUNT_MAX) { return; }   // buffer full: drop instead of writing past the end

    XMFLOAT3* v = &g_LineBuffer[g_LineCount * 2];
    v[0] = a;
    v[1] = b;
    g_LineCount++;
}

void Collision3D_Debug_Add(const CollisionAABB& aabb)
{
    const XMFLOAT3& c = aabb.GetCenter();
    const float hx = aabb.GetHalfWidth();
    const float hy = aabb.GetHalfHeight();
    const float hz = aabb.GetHalfDepth();

    // 8 corners: bottom 4 (y-), then top 4 (y+)
    const XMFLOAT3 p[8] = {
        { c.x - hx, c.y - hy, c.z - hz },   // 0 bottom front-left
        { c.x + hx, c.y - hy, c.z - hz },   // 1 bottom front-right
        { c.x + hx, c.y - hy, c.z + hz },   // 2 bottom back-right
        { c.x - hx, c.y - hy, c.z + hz },   // 3 bottom back-left
        { c.x - hx, c.y + hy, c.z - hz },   // 4 top front-left
        { c.x + hx, c.y + hy, c.z - hz },   // 5 top front-right
        { c.x + hx, c.y + hy, c.z + hz },   // 6 top back-right
        { c.x - hx, c.y + hy, c.z + hz },   // 7 top back-left
    };

    // 12 edges as corner index pairs
    static constexpr int EDGE[12][2] = {
        {0,1},{1,2},{2,3},{3,0},   // bottom square
        {4,5},{5,6},{6,7},{7,4},   // top square
        {0,4},{1,5},{2,6},{3,7},   // vertical pillars
    };

    for (const auto& e : EDGE)
    {
        AddLine(p[e[0]], p[e[1]]);
    }
}

void Collision3D_Debug_Draw()
{
    if (g_LineCount == 0) { return; }

    // copy this frame's lines into the GPU buffer
    D3D11_MAPPED_SUBRESOURCE msr{};
    Direct3D_GetDeviceContext()->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, g_LineBuffer, sizeof(XMFLOAT3) * g_LineCount * 2);
    Direct3D_GetDeviceContext()->Unmap(g_pVertexBuffer, 0);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	Direct3D_GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    Direct3D_GetDeviceContext()->OMSetDepthStencilState(g_pDepthStateOff, 0);

	Direct3D_GetDeviceContext()->Draw(g_LineCount * 2, 0);

	g_LineCount = 0;
}
