cbuffer WorldBuffer : register(b0)
{
    float4x4 world;
    float4x4 worldIT;
};

cbuffer ViewBuffer : register(b1)
{
    float4x4 view;
};

cbuffer ProjectionBuffer : register(b2)
{
    float4x4 projection;
};

cbuffer UVBuffer : register(b3)
{
    float4x4 uvMatrix;
};

struct VS_IN
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    
    float4 posW = mul(input.position, world);
    float4 posWV = mul(posW, view);
    output.position = mul(posWV, projection);
    
    // output = mul(input.posL, mul(world, mul(view, proj)))

    output.uv = mul(float4(input.uv, 0.0f, 1.0f), uvMatrix).xy;

    return output;
}