#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>

#include "structures.h"
#include "light.h"

using namespace DirectX;

static const XMFLOAT4 Vertices[] = {
    {0, -1, -1, 1},
    {0,  1, -1, 1},
    {0,  1,  1, 1},
    {0, -1,  1, 1}
};

class Plane {
public:
    HRESULT init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth,
        int screenHeight, UINT cnt, const std::vector<XMFLOAT4> colors);
    void realize();
    void resize(int screenWidth, int screenHeight) {};
    void render(ID3D11DeviceContext* context);
    bool frame(ID3D11DeviceContext* context, const std::vector<XMMATRIX>& worldMatricies,
        XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, const Light& lights);
private:
    float distToPlane(XMMATRIX worldMatrix, XMFLOAT3 cameraPos);

    ID3D11VertexShader* g_pVertexShader = nullptr;
    ID3D11PixelShader* g_pPixelShader = nullptr;
    ID3D11InputLayout* g_pVertexLayout = nullptr;

    ID3D11Buffer* g_pVertexBuffer = nullptr;
    ID3D11Buffer* g_pIndexBuffer = nullptr;
    ID3D11Buffer* g_pSceneMatrixBuffer = nullptr;
    ID3D11Buffer* g_LightConstantBuffer = nullptr;
    ID3D11RasterizerState* g_pRasterizerState = nullptr;
    ID3D11DepthStencilState* g_pDepthState = nullptr;
    ID3D11BlendState* g_pTransBlendState = nullptr;

    std::vector<ID3D11Buffer*> g_pWorldMatrixBuffers = std::vector<ID3D11Buffer*>(1, nullptr);
    std::vector<UINT> renderOrder;

    std::vector<XMFLOAT4> colors;
};
