#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>

#include "texture.h"
#include "structures.h"
#include "light.h"

using namespace DirectX;

struct Frustum
{
	float screenDepth;
	float planes[6][4];
};

class Cube {
public:
	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight,
		std::vector<const wchar_t*> diffPaths, const wchar_t* normalPath, float shines, const std::vector<XMFLOAT4>& positions);
	void realize();
	void resize(int screenWidth, int screenHeight) {};
	void render(ID3D11DeviceContext* context);
	bool frame(ID3D11DeviceContext* context, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix,
		XMFLOAT3& cameraPos, const Light& lights, bool fixFrustumCulling);

private:
	void getFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	bool isInFrustum(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);

	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;
	ID3D11InputLayout* g_pVertexLayout = nullptr;

	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_pIndexBuffer = nullptr;
	ID3D11Buffer* g_pGeomBuffer = nullptr;
	ID3D11Buffer* g_LightConstantBuffer = nullptr;
	ID3D11Buffer* g_pSceneMatrixBuffer = nullptr;
	ID3D11RasterizerState* g_pRasterizerState = nullptr;
	ID3D11SamplerState* g_pSamplerState = nullptr;
	ID3D11DepthStencilState* g_pDepthState = nullptr;

	std::vector<Texture> cubesTextures;
	std::vector<CubeModel> cubesModelVector;
	std::vector<int> cubesIndexies;

	Frustum frustum;
	float angle_velocity = XM_PIDIV2;
};
