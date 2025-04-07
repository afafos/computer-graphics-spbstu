#include "CubeCB.hlsli"

cbuffer CullingParams : register(b0)
{
    uint4 numShapes;
    float4 bbMin[MAX_CUBES];
    float4 bbMax[MAX_CUBES];
}

RWStructuredBuffer<uint> indirectArgs : register(u0);
RWStructuredBuffer<uint4> objectsIds : register(u1);

bool IsInFrustum(in float4 planes[6], in float3 bbMin, in float3 bbMax)
{
    for (int i = 0; i < 6; i++)
    {
        if ((planes[i].x * bbMin.x) + (planes[i].y * bbMin.y) + (planes[i].z * bbMin.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMax.x) + (planes[i].y * bbMin.y) + (planes[i].z * bbMin.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMin.x) + (planes[i].y * bbMin.y) + (planes[i].z * bbMin.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMax.x) + (planes[i].y * bbMax.y) + (planes[i].z * bbMin.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMin.x) + (planes[i].y * bbMin.y) + (planes[i].z * bbMax.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMax.x) + (planes[i].y * bbMin.y) + (planes[i].z * bbMax.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMin.x) + (planes[i].y * bbMax.y) + (planes[i].z * bbMax.z) + (planes[i].w * 1.0f) >= 0.0f ||
        (planes[i].x * bbMax.x) + (planes[i].y * bbMax.y) + (planes[i].z * bbMax.z) + (planes[i].w * 1.0f) >= 0.0f)
            continue;
        else
            return false;
    }

    return true;
}

[numthreads(64, 1, 1)]
void main(uint3 globalThreadId : SV_DispatchThreadID)
{
    if (globalThreadId.x >= numShapes.x)
    {
        return;
    }
    if (IsInFrustum(planes, bbMin[globalThreadId.x].xyz, bbMax[globalThreadId.x].xyz))
    {
        uint id = 0;
        InterlockedAdd(indirectArgs[1], 1, id);
        objectsIds[id] = uint4(globalThreadId.x, 0, 0, 0);
    }
}