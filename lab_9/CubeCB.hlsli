#include "Consts.h"

struct CubeGeomBuffer
{
    float4x4 worldMatrix;
    float4x4 norm;
    float4 cubeParams;
};

cbuffer CubeGeomBuffers : register(b0)
{
    CubeGeomBuffer geomBuffers[MAX_CUBES];
};

cbuffer SceneCB : register(b1)
{
    float4x4 viewProjectionMatrix;
    float4 planes[6];
};

cbuffer IndexBuffer : register(b2)
{
    uint4 objectID[MAX_CUBES];
}