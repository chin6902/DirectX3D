Texture2D major_texture : register(t0);
SamplerState major_sampler : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 light_directionW;
    float4 ambient_color;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PS_IN input) : SV_TARGET
{
    // Calculate directional light
    //float L = (dot(normalize(input.normalW), -light_directionW) + 1.0f) * 0.5f;
    float L = max(dot(normalize(input.normalW), -light_directionW), 0.0f);
    
    float3 diffuse = major_texture.Sample(major_sampler, input.uv).rgb * input.color.rgb * L;
    float3 ambient = major_texture.Sample(major_sampler, input.uv).rgb * ambient_color.rgb;
    float alpha = input.color.a;
    
    return float4(diffuse + ambient, alpha);
}
