// 2D sprite vertex shader
// Uses the same constant buffer slots as the 3D shaders, but only needs the projection.
// b0 (world) and b1 (view) are not used: sprite vertices are already in screen pixels.

cbuffer ProjectionBuffer : register(b2)
{
    float4x4 projection;
};

struct VS_IN
{
    float4 position : POSITION0; // float3 in the vertex buffer, w is filled in as 1
    float2 uv       : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;

    output.position = mul(input.position, projection);
    output.uv = input.uv;

    return output;
}
