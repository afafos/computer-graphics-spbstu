#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>

#include "structures.h"
#include "texture.h"

using namespace DirectX;

class Skybox {
public:
	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight);
	void realize();
	void resize(int screenWidth, int screenHeight);
	void render(ID3D11DeviceContext* context);
	bool frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

private:
	void generateSphere(UINT LatLines, UINT LongLines, std::vector<SimpleVertex>& vertices, std::vector<UINT>& indices);

	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_pIndexBuffer = nullptr;
	ID3D11Buffer* g_pWorldMatrixBuffer = nullptr;
	ID3D11Buffer* g_pSceneMatrixBuffer = nullptr;
	ID3D11RasterizerState* g_pRasterizerState = nullptr;
	ID3D11SamplerState* g_pSamplerState = nullptr;

	ID3D11InputLayout* g_pVertexLayout = nullptr;
	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;

	Texture texture;

	UINT numSphereVertices = 0;
	UINT numSphereFaces = 0;
	float radius = 1.0f;
};