#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>

#include "texture.h"
#include "structures.h"

using namespace DirectX;

class Cube {
public:
	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight, UINT count);
	void realize();
	void resize(int screenWidth, int screenHeight) {};
	void render(ID3D11DeviceContext* context);
	bool frame(ID3D11DeviceContext* context, const std::vector<XMMATRIX>& worldMatricies, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

private:
	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;
	ID3D11InputLayout* g_pVertexLayout = nullptr;

	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_pIndexBuffer = nullptr;
	ID3D11Buffer* g_pSceneMatrixBuffer = nullptr;
	ID3D11RasterizerState* g_pRasterizerState = nullptr;
	ID3D11SamplerState* g_pSamplerState = nullptr;
	ID3D11DepthStencilState* g_pDepthState = nullptr;

	std::vector<ID3D11Buffer*> g_pWorldMatrixBuffers = std::vector<ID3D11Buffer*>(1, nullptr);

	Texture texture;
};