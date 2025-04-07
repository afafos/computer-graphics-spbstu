#include "TransparentCB.hlsli"
#include "CalculateColor.hlsli"

struct VS_INPUT
{
    float4 position : POSITION;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    output.worldPos = mul(worldMatrix, input.position);
    output.position = mul(viewProjectionMatrix, output.worldPos);
    return output;
}