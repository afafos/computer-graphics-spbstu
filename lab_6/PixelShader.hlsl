#include "CalculateColor.hlsli"

cbuffer WorldMatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4 shine;
};

Texture2D tex : register(t0);
Texture2D normal : register(t1);

SamplerState smplr : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

float4 main(PS_INPUT input) : SV_Target0
{
    input.binormal = normalize(input.binormal);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);
    float3 ambient = ambientColor.xyz * tex.Sample(smplr, input.uv).xyz;
    float3 localNorm = normal.Sample(smplr, input.uv).xyz * 2.0 - 1.0;
    float3 norm = localNorm.x * input.tangent + localNorm.y * input.binormal + localNorm.z * input.normal;
    return float4(CalculateColor(ambient, norm, input.worldPos.xyz, shine.x, false), 1.0);
}