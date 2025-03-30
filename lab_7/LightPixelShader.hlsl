#include "LightBuffers.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    uint instanceId : SV_InstanceID;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return lightsGeomBuffer[input.instanceId].color;
}