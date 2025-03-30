#include "CubeCB.hlsli"
#include "CalculateColor.hlsli"

Texture2DArray tex : register(t0);
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
    nointerpolation uint instanceId : INST_ID;
};

float4 main(PS_INPUT input) : SV_Target0
{
    input.binormal = normalize(input.binormal);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);
    
    float3 ambient = 5.0 * ambientColor.xyz * tex.Sample(smplr, float3(input.uv, geomBuffers[input.instanceId].cubeParams.z)).xyz;
    
    float3 norm = float3(0.0f, 0.0f, 0.0f);
    if (geomBuffers[input.instanceId].cubeParams.w > 0.0f)
    {
        float3 localNorm = normal.Sample(smplr, input.uv).xyz * 2.0 - 1.0;
        norm = localNorm.x * normalize(input.tangent) + localNorm.y * input.binormal + localNorm.z * normalize(input.normal);
    }
    else
        norm = input.normal;
    
    return float4(CalculateColor(ambient, norm, input.worldPos.xyz, geomBuffers[input.instanceId].cubeParams.x, false), 1.0);
}