#pragma once

#include <directxmath.h>

#include "Consts.h"

using namespace DirectX;

struct WorldMatrixBuffer {
	XMMATRIX worldMatrix;
	XMFLOAT4 color;
};

struct SceneMatrixBuffer {
	XMMATRIX viewProjectionMatrix;
};

struct CubeSceneMatrixBuffer {
	XMMATRIX viewProjectionMatrix;
	XMINT4 indexBuffer[MAX_CUBES];
};

struct GeomBuffer {
	XMMATRIX worldMatrix;
	XMMATRIX norm;
	XMFLOAT4 params;
};

struct CubeModel {
	XMFLOAT4 pos;
	XMFLOAT4 params;
};

struct TexVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
};

struct LightableCB {
	XMFLOAT4 cameraPos;
	XMINT4 lightCount;
	XMFLOAT4 lightPos[MAX_LIGHTS];
	XMFLOAT4 lightColor[MAX_LIGHTS];
	XMFLOAT4 ambientColor;
};

struct SimpleVertex
{
	float x, y, z;
};

struct SBWorldMatrixBuffer {
	XMMATRIX worldMatrix;
	XMFLOAT4 size;
};

struct SBSceneMatrixBuffer {
	XMMATRIX viewProjectionMatrix;
	XMFLOAT4 cameraPos;
};
