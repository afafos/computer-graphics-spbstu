#include "Consts.h"

cbuffer TransWorldMatrixCB : register(b0)
{
    float4x4 worldMatrix;
    float4 color;
};

cbuffer TransSceneCB : register(b1)
{
    float4x4 viewProjectionMatrix;
};