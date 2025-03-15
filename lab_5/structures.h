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
	float x, y, z;
	float u, v;
};