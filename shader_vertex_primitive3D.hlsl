cbuffer ViewBuffer : register(b1)
{
    float4x4 view;
};

cbuffer ProjectionBuffer : register(b2)
{
    float4x4 projection;
};

float4 main(float4 posW : POSITION0) : SV_POSITION
{
    return mul(posW, mul(view, projection));
}