cbuffer WorldMatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4 color;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(color.xyz, 0.5f);
}