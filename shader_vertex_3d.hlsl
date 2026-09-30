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

struct VS_IN
{
    float4 position : POSITION0;
    float4 normalL  : NORMAL0;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normalW  : NORMAL0;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    
    float4 posW = mul(input.position, world);
    float4 posWV = mul(posW, view);
    output.position = mul(posWV, projection);

    float3 n = mul(float4(input.normalL.xyz, 0.0f), worldIT).xyz;
    output.normalW = float4(normalize(n), 0.0f);
    
    output.color = input.color;
    output.uv = input.uv;

    return output;
}