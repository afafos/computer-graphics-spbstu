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
	XMFLOAT4 planes[6];
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
	int getRenderedCubesCount() { return countOfRenderedCubes; };

private:
	HRESULT initQuery(ID3D11Device* device);
	void readQueries(ID3D11DeviceContext* context);
	void getFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;
	ID3D11InputLayout* g_pVertexLayout = nullptr;
	ID3D11ComputeShader* g_pCullShader = nullptr;

	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_pIndexBuffer = nullptr;
	ID3D11Buffer* g_pGeomBuffer = nullptr;
	ID3D11Buffer* g_pCullingParams = nullptr;
	ID3D11Buffer* g_LightConstantBuffer = nullptr;
	ID3D11Buffer* g_pSceneMatrixBuffer = nullptr;
	ID3D11RasterizerState* g_pRasterizerState = nullptr;
	ID3D11SamplerState* g_pSamplerState = nullptr;
	ID3D11DepthStencilState* g_pDepthState = nullptr;

	ID3D11Buffer* g_pInderectArgsSrc = nullptr;
	ID3D11Buffer* g_pInderectArgs = nullptr;
	ID3D11UnorderedAccessView* g_pInderectArgsUAV = nullptr;
	ID3D11Buffer* g_pGeomBufferInstVis = nullptr;
	ID3D11Buffer* g_pGeomBufferInstVisGpu = nullptr;
	ID3D11UnorderedAccessView* g_pGeomBufferInstVisGpu_UAV = nullptr;

	std::vector<Texture> cubesTextures;
	std::vector<CubeModel> cubesModelVector;
	std::vector<int> cubesIndexies;

	Frustum frustum;
	float angle_velocity = XM_PIDIV2;

	int countOfRenderedCubes = MAX_CUBES;

	UINT curFrame = 0;
	UINT lastCompletedFrame = 0;
	ID3D11Query* queries[MAX_QUERY];
};
