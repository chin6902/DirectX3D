Texture2D major_texture : register(t0);
SamplerState major_sampler : register(s0);

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PS_IN input) : SV_TARGET
{
    float4 c = major_texture.Sample(major_sampler, input.uv) * color;

    clip(c.a - 0.01f);
    
    return c;
}