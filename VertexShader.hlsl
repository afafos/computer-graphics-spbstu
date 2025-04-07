#include "CubeCB.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint instanceId : SV_InstanceID;
};

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

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    unsigned int idx = objectID[input.instanceId].x;
    output.worldPos = mul(geomBuffers[idx].worldMatrix, float4(input.position, 1.0f));
    output.position = mul(viewProjectionMatrix, output.worldPos);
    output.normal = mul(geomBuffers[idx].norm, float4(input.normal, 0.0f)).xyz;
    output.tangent = mul(geomBuffers[idx].norm, float4(input.tangent, 0.0f)).xyz;
    output.binormal = normalize(cross(output.normal, output.tangent));
    output.uv = input.uv;
    output.instanceId = idx;
    return output;
}
