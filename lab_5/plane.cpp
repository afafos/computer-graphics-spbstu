#include <algorithm>

#include "plane.h"

HRESULT Plane::init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight, UINT cnt, const std::vector<XMFLOAT4> colors) {
    this->colors = colors;

    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = D3DReadFileToBlob(L"TransparentVertexShader.cso", &pVSBlob);
    if (FAILED(hr)) {
        MessageBox(nullptr,
            L"TransparentVertexShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if (FAILED(hr)) {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &g_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    context->IASetInputLayout(g_pVertexLayout);

    ID3DBlob* pPSBlob = nullptr;
    hr = D3DReadFileToBlob(L"TransparentPixelShader.cso", &pPSBlob);
    if (FAILED(hr)) {
        MessageBox(nullptr,
            L"TransparentPixelShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    USHORT indices[] = {
          0, 2, 1, 0, 3, 2,
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(Vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = &Vertices;
    InitData.SysMemPitch = sizeof(Vertices);
    InitData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC bd1;
    ZeroMemory(&bd1, sizeof(bd1));
    bd1.Usage = D3D11_USAGE_IMMUTABLE;
    bd1.ByteWidth = sizeof(indices);
    bd1.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd1.CPUAccessFlags = 0;
    bd1.MiscFlags = 0;
    bd1.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA InitData1;
    ZeroMemory(&InitData1, sizeof(InitData1));
    InitData1.pSysMem = &indices;
    InitData1.SysMemPitch = sizeof(indices);
    InitData1.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&bd1, &InitData1, &g_pIndexBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC descWMB = {};
    descWMB.ByteWidth = sizeof(WorldMatrixBuffer);
    descWMB.Usage = D3D11_USAGE_DEFAULT;
    descWMB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descWMB.CPUAccessFlags = 0;
    descWMB.MiscFlags = 0;
    descWMB.StructureByteStride = 0;

    WorldMatrixBuffer worldMatrixBuffer;
    worldMatrixBuffer.worldMatrix = DirectX::XMMatrixIdentity();

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &worldMatrixBuffer;
    data.SysMemPitch = sizeof(worldMatrixBuffer);
    data.SysMemSlicePitch = 0;

    g_pWorldMatrixBuffers = std::vector<ID3D11Buffer*>(cnt, nullptr);
    renderOrder = std::vector<UINT>(cnt, 0);
    for (UINT i = 0; i < cnt; i++) {
        hr = device->CreateBuffer(&descWMB, &data, &g_pWorldMatrixBuffers[i]);
        if (FAILED(hr))
            return hr;
    }

    D3D11_BUFFER_DESC descSMB = {};
    descSMB.ByteWidth = sizeof(SceneMatrixBuffer);
    descSMB.Usage = D3D11_USAGE_DYNAMIC;
    descSMB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descSMB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    descSMB.MiscFlags = 0;
    descSMB.StructureByteStride = 0;

    hr = device->CreateBuffer(&descSMB, nullptr, &g_pSceneMatrixBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_RASTERIZER_DESC descRastr = {};
    descRastr.FillMode = D3D11_FILL_SOLID;
    descRastr.CullMode = D3D11_CULL_NONE;
    descRastr.FrontCounterClockwise = false;
    descRastr.DepthBias = 0;
    descRastr.SlopeScaledDepthBias = 0.0f;
    descRastr.DepthBiasClamp = 0.0f;
    descRastr.DepthClipEnable = true;
    descRastr.ScissorEnable = false;
    descRastr.MultisampleEnable = false;
    descRastr.AntialiasedLineEnable = false;

    hr = device->CreateRasterizerState(&descRastr, &g_pRasterizerState);
    if (FAILED(hr))
        return hr;

    D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    dsDesc.StencilEnable = FALSE;

    hr = device->CreateDepthStencilState(&dsDesc, &g_pDepthState);
    if (FAILED(hr))
        return hr;

    D3D11_BLEND_DESC descBS = { 0 };
    descBS.AlphaToCoverageEnable = false;
    descBS.IndependentBlendEnable = false;
    descBS.RenderTarget[0].BlendEnable = true;
    descBS.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    descBS.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    descBS.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    descBS.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
    descBS.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    descBS.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    descBS.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

    hr = device->CreateBlendState(&descBS, &g_pTransBlendState);

    return S_OK;
}

void Plane::realize() {
    if (g_pTransBlendState) g_pTransBlendState->Release();
    if (g_pRasterizerState) g_pRasterizerState->Release();

    for (auto& buffer : g_pWorldMatrixBuffers)
        if (buffer)
            buffer->Release();

    if (g_pDepthState) g_pDepthState->Release();
    if (g_pSceneMatrixBuffer) g_pSceneMatrixBuffer->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
}

void Plane::render(ID3D11DeviceContext* context) {
    context->OMSetDepthStencilState(g_pDepthState, 0);
    context->RSSetState(g_pRasterizerState);

    context->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    ID3D11Buffer* vertexBuffers[] = { g_pVertexBuffer };
    UINT stride = sizeof(XMFLOAT4);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

    context->IASetInputLayout(g_pVertexLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(g_pVertexShader, nullptr, 0);
    context->VSSetConstantBuffers(1, 1, &g_pSceneMatrixBuffer);
    context->PSSetShader(g_pPixelShader, nullptr, 0);
    context->OMSetBlendState(g_pTransBlendState, nullptr, 0xFFFFFFFF);

    for (auto& i : renderOrder) {
        context->VSSetConstantBuffers(0, 1, &g_pWorldMatrixBuffers[i]);
        context->PSSetConstantBuffers(0, 1, &g_pWorldMatrixBuffers[i]);

        context->DrawIndexed(6, 0, 0);
    }
}


float Plane::distToPlane(XMMATRIX worldMatrix, XMFLOAT3 cameraPos) {
    XMFLOAT4 rectVert[4];
    float maxDist = -D3D11_FLOAT32_MAX;

    std::copy(Vertices, Vertices + 4, rectVert);
    for (int i = 0; i < 4; i++) {
        XMStoreFloat4(&rectVert[i], XMVector4Transform(XMLoadFloat4(&rectVert[i]), worldMatrix));
        float dist = (rectVert[i].x * cameraPos.x) + (rectVert[i].y * cameraPos.y) + (rectVert[i].z * cameraPos.z);
        maxDist = max(maxDist, dist);
    }

    return maxDist;
}

bool Plane::frame(ID3D11DeviceContext* context, const std::vector<XMMATRIX>& worldMatricies, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    WorldMatrixBuffer worldMatrixBuffer;

    for (int i = 0; i < worldMatricies.size() && i < g_pWorldMatrixBuffers.size(); i++) {
        worldMatrixBuffer.worldMatrix = worldMatricies[i];
        worldMatrixBuffer.color = colors[i];
        context->UpdateSubresource(g_pWorldMatrixBuffers[i], 0, nullptr, &worldMatrixBuffer, 0, 0);
    }
    std::vector<std::pair<float, UINT>> dists_indxs = std::vector<std::pair<float, UINT>>(worldMatricies.size());
    for (UINT i = 0; i < worldMatricies.size(); i++)
        dists_indxs[i] = std::pair<float, UINT>(distToPlane(worldMatricies[i], cameraPos), i);
    std::sort(dists_indxs.begin(), dists_indxs.end(), [](std::pair<float, UINT> a, std::pair<float, UINT> b) {return a.first < b.first;});
    for (UINT i = 0; i < worldMatricies.size(); i++)
        renderOrder[i] = dists_indxs[i].second;

    D3D11_MAPPED_SUBRESOURCE subresource;
    HRESULT hr = context->Map(g_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    if (FAILED(hr))
        return FAILED(hr);

    SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
    sceneBuffer.viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
    context->Unmap(g_pSceneMatrixBuffer, 0);

    return S_OK;
}