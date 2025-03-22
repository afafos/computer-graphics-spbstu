#pragma once

#include <directxmath.h>

using namespace DirectX;

struct WorldMatrixBuffer {
	XMMATRIX worldMatrix;
	XMFLOAT4 color;
};

struct SceneMatrixBuffer {
	XMMATRIX viewProjectionMatrix;
};

struct TexVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
};

struct LightableSceneMatrixBuffer {
	XMMATRIX viewProjectionMatrix;
	XMFLOAT4 cameraPos;
	XMINT4 lightCount;
	XMFLOAT4 lightPos[10];
	XMFLOAT4 lightColor[10];
	XMFLOAT4 ambientColor;
};

struct SimpleVertex
{
	float x, y, z;
};

struct SBWorldMatrixBuffer
{
	XMMATRIX worldMatrix;
	XMFLOAT4 size;
};

struct SBSceneMatrixBuffer
{
	XMMATRIX viewProjectionMatrix;
	XMFLOAT4 cameraPos;
};