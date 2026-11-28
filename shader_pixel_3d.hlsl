Texture2D major_texture : register(t0);
SamplerState major_sampler : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 light_directionW;
    float4 diffuse_color;
    float4 ambient_color;
};

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normalW  : NORMAL0;
    float4 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

float4 main(PS_IN input) : SV_TARGET
{
    // Calculate directional light
    //float L = (dot(normalize(input.normalW), -light_directionW) + 1.0f) * 0.5f;
    float3 light_color = diffuse_color.rgb * max(dot(normalize(input.normalW.xyz), -light_directionW.xyz), 0.0f);
    
    float4 tex = major_texture.Sample(major_sampler, input.uv); 
    float3 albedo = tex.rgb * color.rgb;

    float3 diffuse = albedo * input.color.rgb * light_color;
    float3 ambient = albedo * ambient_color.rgb;
    float alpha = tex.a * input.color.a * color.a; // texture * vertex * constant

    return float4(diffuse + ambient, alpha);
}
