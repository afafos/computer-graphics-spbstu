#include "LightBuffers.hlsli"

struct VS_INPUT
{
    float4 position : POSITION;
    uint instanceId : SV_InstanceID;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    uint instanceId : SV_InstanceID;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    unsigned int idx = input.instanceId;
    output.instanceId = idx;

    output.position = mul(viewProjectionMatrix, mul(lightsGeomBuffer[idx].worldMatrix, input.position));

    return output;
}