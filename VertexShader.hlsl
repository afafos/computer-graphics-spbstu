#include "SceneCB.hlsli"

cbuffer WorldMatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4 shine;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    output.worldPos = mul(worldMatrix, float4(input.position, 1.0f));
    output.position = mul(viewProjectionMatrix, output.worldPos);
    output.normal = mul(worldMatrix, float4(input.normal, 0.0f)).xyz;
    output.tangent = mul(worldMatrix, float4(input.tangent, 0.0f)).xyz;
    output.binormal = normalize(cross(output.normal, output.tangent));
    output.uv = input.uv;

    return output;
}
