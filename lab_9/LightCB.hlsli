#include "Consts.h"

cbuffer LightCB : register(b2)
{
    float4 cameraPos;
    int4 lightCount;
    float4 lightPos[MAX_LIGHTS];
    float4 lightColor[MAX_LIGHTS];
    float4 ambientColor;
};