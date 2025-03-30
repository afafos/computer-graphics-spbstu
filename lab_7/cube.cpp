#include "cube.h"
#include "timer.h"

HRESULT Cube::init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight,
    std::vector<const wchar_t*> diffPaths, const wchar_t* normalPath, float shines, const std::vector<XMFLOAT4>& positions) {
    
    assert(positions.size() == MAX_CUBES);

    frustum.screenDepth = 0.1f;

    for (int i = 0; i < MAX_CUBES; i++) {
        CubeModel tmp;
        float textureIndex = (float)(rand() % diffPaths.size());
        tmp.pos = positions[i];
        tmp.params = XMFLOAT4(shines, (float)(rand() % 10 - 5), textureIndex, textureIndex > 0.0f ? 0.0f : 1.0f);
        cubesModelVector.push_back(tmp);
    }

    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = D3DReadFileToBlob(L"VertexShader.cso", &pVSBlob);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"VertexShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if (FAILED(hr)) {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    context->IASetInputLayout(g_pVertexLayout);

    ID3DBlob* pPSBlob = nullptr;
    hr = D3DReadFileToBlob(L"PixelShader.cso", &pPSBlob);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"PixelShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    cubesTextures = std::vector<Texture>(2);
    hr = cubesTextures[0].initArray(device, context, diffPaths);
    hr = cubesTextures[1].init(device, context, normalPath);
    if (FAILED(hr))
        return hr;

    TexVertex vertices[] = {
        {{-0.5, -0.5,  0.5}, {0, 1}, {0, -1, 0}, {1, 0, 0}},
        {{ 0.5, -0.5,  0.5}, {1, 1}, {0, -1, 0}, {1, 0, 0}},
        {{ 0.5, -0.5, -0.5}, {1, 0}, {0, -1, 0}, {1, 0, 0}},
        {{-0.5, -0.5, -0.5}, {0, 0}, {0, -1, 0}, {1, 0, 0}},

        {{-0.5,  0.5, -0.5}, {1, 1}, {0, 1, 0}, {1, 0, 0}},
        {{ 0.5,  0.5, -0.5}, {0, 1}, {0, 1, 0}, {1, 0, 0}},
        {{ 0.5,  0.5,  0.5}, {0, 0}, {0, 1, 0}, {1, 0, 0}},
        {{-0.5,  0.5,  0.5}, {1, 0}, {0, 1, 0}, {1, 0, 0}},

        {{ 0.5, -0.5, -0.5}, {0, 1}, {1, 0, 0}, {0, 0, 1}},
        {{ 0.5, -0.5,  0.5}, {1, 1}, {1, 0, 0}, {0, 0, 1}},
        {{ 0.5,  0.5,  0.5}, {1, 0}, {1, 0, 0}, {0, 0, 1}},
        {{ 0.5,  0.5, -0.5}, {0, 0}, {1, 0, 0}, {0, 0, 1}},

        {{-0.5, -0.5,  0.5}, {0, 1}, {-1, 0, 0}, {0, 0, -1}},
        {{-0.5, -0.5, -0.5}, {1, 1}, {-1, 0, 0}, {0, 0, -1}},
        {{-0.5,  0.5, -0.5}, {1, 0}, {-1, 0, 0}, {0, 0, -1}},
        {{-0.5,  0.5,  0.5}, {0, 0}, {-1, 0, 0}, {0, 0, -1}},

        {{ 0.5, -0.5,  0.5}, {1, 1}, {0, 0, 1}, {-1, 0, 0}},
        {{-0.5, -0.5,  0.5}, {0, 1}, {0, 0, 1}, {-1, 0, 0}},
        {{-0.5,  0.5,  0.5}, {0, 0}, {0, 0, 1}, {-1, 0, 0}},
        {{ 0.5,  0.5,  0.5}, {1, 0}, {0, 0, 1}, {-1, 0, 0}},

        {{-0.5, -0.5, -0.5}, {1, 1}, {0, 0, -1}, {1, 0, 0}},
        {{ 0.5, -0.5, -0.5}, {0, 1}, {0, 0, -1}, {1, 0, 0}},
        {{ 0.5,  0.5, -0.5}, {0, 0}, {0, 0, -1}, {1, 0, 0}},
        {{-0.5,  0.5, -0.5}, {1, 0}, {0, 0, -1}, {1, 0, 0}}
    };

    USHORT indices[] = {
          0, 2, 1, 0, 3, 2,
          4, 6, 5, 4, 7, 6,
          8, 10, 9, 8, 11, 10,
          12, 14, 13, 12, 15, 14,
          16, 18, 17, 16, 19, 18,
          20, 22, 21, 20, 23, 22
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = &vertices;
    InitData.SysMemPitch = sizeof(vertices);
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
    descWMB.ByteWidth = sizeof(GeomBuffer) * MAX_CUBES;
    descWMB.Usage = D3D11_USAGE_DEFAULT;
    descWMB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descWMB.CPUAccessFlags = 0;
    descWMB.MiscFlags = 0;
    descWMB.StructureByteStride = 0;

    GeomBuffer geomBufferInst[MAX_CUBES];
    for (int i = 0; i < MAX_CUBES; i++) {
        geomBufferInst[i].worldMatrix = XMMatrixTranslation(cubesModelVector[i].pos.x, cubesModelVector[i].pos.y, cubesModelVector[i].pos.z);
        geomBufferInst[i].norm = geomBufferInst[i].worldMatrix;
        geomBufferInst[i].params = cubesModelVector[i].params;
    }

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &geomBufferInst;
    data.SysMemPitch = sizeof(geomBufferInst);
    data.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&descWMB, &data, &g_pGeomBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC descSMB = {};
    descSMB.ByteWidth = sizeof(CubeSceneMatrixBuffer);
    descSMB.Usage = D3D11_USAGE_DYNAMIC;
    descSMB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descSMB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    descSMB.MiscFlags = 0;
    descSMB.StructureByteStride = 0;

    hr = device->CreateBuffer(&descSMB, nullptr, &g_pSceneMatrixBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC descLCB = {};
    descLCB.ByteWidth = sizeof(LightableCB);
    descLCB.Usage = D3D11_USAGE_DYNAMIC;
    descLCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descLCB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    descLCB.MiscFlags = 0;
    descLCB.StructureByteStride = 0;

    hr = device->CreateBuffer(&descLCB, nullptr, &g_LightConstantBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_RASTERIZER_DESC descRastr = {};
    descRastr.FillMode = D3D11_FILL_SOLID;
    descRastr.CullMode = D3D11_CULL_BACK;
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

    D3D11_SAMPLER_DESC descSmplr = {};
    descSmplr.Filter = D3D11_FILTER_ANISOTROPIC;
    descSmplr.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    descSmplr.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    descSmplr.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    descSmplr.MinLOD = -D3D11_FLOAT32_MAX;
    descSmplr.MaxLOD = D3D11_FLOAT32_MAX;
    descSmplr.MipLODBias = 0.0f;
    descSmplr.MaxAnisotropy = 16;
    descSmplr.ComparisonFunc = D3D11_COMPARISON_NEVER;
    descSmplr.BorderColor[0] =
        descSmplr.BorderColor[1] =
        descSmplr.BorderColor[2] =
        descSmplr.BorderColor[3] = 1.0f;

    hr = device->CreateSamplerState(&descSmplr, &g_pSamplerState);
    if (FAILED(hr))
        return hr;

    D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    dsDesc.StencilEnable = FALSE;

    hr = device->CreateDepthStencilState(&dsDesc, &g_pDepthState);
    if (FAILED(hr))
        return hr;

    return S_OK;
}


void Cube::realize() {
    for (auto& tex : cubesTextures)
        tex.realize();

    if (g_pSamplerState) g_pSamplerState->Release();
    if (g_pRasterizerState) g_pRasterizerState->Release();

    if (g_pGeomBuffer) g_pGeomBuffer->Release();
    if (g_LightConstantBuffer) g_LightConstantBuffer->Release();

    if (g_pDepthState) g_pDepthState->Release();
    if (g_pSceneMatrixBuffer) g_pSceneMatrixBuffer->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
}

void Cube::render(ID3D11DeviceContext* context) {
    context->OMSetDepthStencilState(g_pDepthState, 0);
    context->RSSetState(g_pRasterizerState);

    context->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    ID3D11SamplerState* samplers[] = { g_pSamplerState };
    context->PSSetSamplers(0, 1, samplers);

    ID3D11ShaderResourceView* resources[] = {
        cubesTextures[0].getTexture(),
        cubesTextures[1].getTexture()
    };
    context->PSSetShaderResources(0, 2, resources);

    ID3D11Buffer* vertexBuffers[] = { g_pVertexBuffer };
    UINT strides[] = { sizeof(TexVertex) };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    context->IASetInputLayout(g_pVertexLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(g_pVertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &g_pGeomBuffer);
    context->VSSetConstantBuffers(1, 1, &g_pSceneMatrixBuffer);

    context->PSSetShader(g_pPixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &g_pGeomBuffer);
    context->PSSetConstantBuffers(1, 1, &g_pSceneMatrixBuffer);
    context->PSSetConstantBuffers(2, 1, &g_LightConstantBuffer);

    context->DrawIndexedInstanced(36, (UINT)cubesIndexies.size(), 0, 0, 0);
}

void Cube::getFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
    XMFLOAT4X4 pMatrix;
    XMStoreFloat4x4(&pMatrix, projectionMatrix);

    float zMinimum = -pMatrix._43 / pMatrix._33;
    float r = frustum.screenDepth / (frustum.screenDepth - zMinimum);

    pMatrix._33 = r;
    pMatrix._43 = -r * zMinimum;
    projectionMatrix = XMLoadFloat4x4(&pMatrix);

    XMMATRIX finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    XMFLOAT4X4 matrix;
    XMStoreFloat4x4(&matrix, finalMatrix);

    frustum.planes[0][0] = matrix._14 + matrix._13;
    frustum.planes[0][1] = matrix._24 + matrix._23;
    frustum.planes[0][2] = matrix._34 + matrix._33;
    frustum.planes[0][3] = matrix._44 + matrix._43;

    float length = sqrtf((frustum.planes[0][0] * frustum.planes[0][0])
        + (frustum.planes[0][1] * frustum.planes[0][1]) + (frustum.planes[0][2] * frustum.planes[0][2]));
    frustum.planes[0][0] /= length;
    frustum.planes[0][1] /= length;
    frustum.planes[0][2] /= length;
    frustum.planes[0][3] /= length;

    frustum.planes[1][0] = matrix._14 - matrix._13;
    frustum.planes[1][1] = matrix._24 - matrix._23;
    frustum.planes[1][2] = matrix._34 - matrix._33;
    frustum.planes[1][3] = matrix._44 - matrix._43;

    length = sqrtf((frustum.planes[1][0] * frustum.planes[1][0])
        + (frustum.planes[1][1] * frustum.planes[1][1]) + (frustum.planes[1][2] * frustum.planes[1][2]));
    frustum.planes[1][0] /= length;
    frustum.planes[1][1] /= length;
    frustum.planes[1][2] /= length;
    frustum.planes[1][3] /= length;

    frustum.planes[2][0] = matrix._14 + matrix._11;
    frustum.planes[2][1] = matrix._24 + matrix._21;
    frustum.planes[2][2] = matrix._34 + matrix._31;
    frustum.planes[2][3] = matrix._44 + matrix._41;

    length = sqrtf((frustum.planes[2][0] * frustum.planes[2][0])
        + (frustum.planes[2][1] * frustum.planes[2][1]) + (frustum.planes[2][2] * frustum.planes[2][2]));
    frustum.planes[2][0] /= length;
    frustum.planes[2][1] /= length;
    frustum.planes[2][2] /= length;
    frustum.planes[2][3] /= length;

    frustum.planes[3][0] = matrix._14 - matrix._11;
    frustum.planes[3][1] = matrix._24 - matrix._21;
    frustum.planes[3][2] = matrix._34 - matrix._31;
    frustum.planes[3][3] = matrix._44 - matrix._41;

    length = sqrtf((frustum.planes[3][0] * frustum.planes[3][0])
        + (frustum.planes[3][1] * frustum.planes[3][1]) + (frustum.planes[3][2] * frustum.planes[3][2]));
    frustum.planes[3][0] /= length;
    frustum.planes[3][1] /= length;
    frustum.planes[3][2] /= length;
    frustum.planes[3][3] /= length;

    frustum.planes[4][0] = matrix._14 - matrix._12;
    frustum.planes[4][1] = matrix._24 - matrix._22;
    frustum.planes[4][2] = matrix._34 - matrix._32;
    frustum.planes[4][3] = matrix._44 - matrix._42;

    length = sqrtf((frustum.planes[4][0] * frustum.planes[4][0])
        + (frustum.planes[4][1] * frustum.planes[4][1]) + (frustum.planes[4][2] * frustum.planes[4][2]));
    frustum.planes[4][0] /= length;
    frustum.planes[4][1] /= length;
    frustum.planes[4][2] /= length;
    frustum.planes[4][3] /= length;

    frustum.planes[5][0] = matrix._14 + matrix._12;
    frustum.planes[5][1] = matrix._24 + matrix._22;
    frustum.planes[5][2] = matrix._34 + matrix._32;
    frustum.planes[5][3] = matrix._44 + matrix._42;

    length = sqrtf((frustum.planes[5][0] * frustum.planes[5][0])
        + (frustum.planes[5][1] * frustum.planes[5][1]) + (frustum.planes[5][2] * frustum.planes[5][2]));
    frustum.planes[5][0] /= length;
    frustum.planes[5][1] /= length;
    frustum.planes[5][2] /= length;
    frustum.planes[5][3] /= length;
}

bool Cube::isInFrustum(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth) {
    for (int i = 0; i < 6; i++) {
        if (((frustum.planes[i][0] * minWidth) + (frustum.planes[i][1] * minHeight) + (frustum.planes[i][2] * minDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * maxWidth) + (frustum.planes[i][1] * minHeight) + (frustum.planes[i][2] * minDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * minWidth) + (frustum.planes[i][1] * maxHeight) + (frustum.planes[i][2] * minDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * maxWidth) + (frustum.planes[i][1] * maxHeight) + (frustum.planes[i][2] * minDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * minWidth) + (frustum.planes[i][1] * minHeight) + (frustum.planes[i][2] * maxDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * maxWidth) + (frustum.planes[i][1] * minHeight) + (frustum.planes[i][2] * maxDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * minWidth) + (frustum.planes[i][1] * maxHeight) + (frustum.planes[i][2] * maxDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f ||
            ((frustum.planes[i][0] * maxWidth) + (frustum.planes[i][1] * maxHeight) + (frustum.planes[i][2] * maxDepth) + (frustum.planes[i][3] * 1.0f)) >= 0.0f)
            continue;
        else
            return false;
    }
    return true;
}

bool Cube::frame(ID3D11DeviceContext* context, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix,
        XMFLOAT3& cameraPos, const Light& lights, bool fixFrustumCulling) {
    auto duration = Timer::GetInstance().Clock();
    GeomBuffer geomBufferInst[MAX_CUBES];
    for (int i = 0; i < MAX_CUBES; i++) {
        geomBufferInst[i].worldMatrix = 
            XMMatrixRotationX((float)duration * cubesModelVector[i].params.x * 0.01f) * 
            XMMatrixTranslation((float)sin(duration) * 0.5f, (float)cos(duration) * 0.5f, (float)sin(duration) * 0.5f) *
            XMMatrixRotationY((float)duration * cubesModelVector[i].params.y * 1.5f) *
            XMMatrixRotationZ((float)(sin(duration * cubesModelVector[i].params.y * 0.30f) * 0.25f)) *
            XMMatrixTranslation((float)sin(duration * angle_velocity * cubesModelVector[i].params.y * 1.0), (float)(sin(duration * cubesModelVector[i].params.y * 0.30) * 0.25f), (float)cos(duration) * 3.0f) *
            XMMatrixTranslation(cubesModelVector[i].pos.x, cubesModelVector[i].pos.y, cubesModelVector[i].pos.z);
        geomBufferInst[i].norm = geomBufferInst[i].worldMatrix;
        geomBufferInst[i].params = cubesModelVector[i].params;
    }

    context->UpdateSubresource(g_pGeomBuffer, 0, nullptr, &geomBufferInst, 0, 0);

    if (!fixFrustumCulling) {
        getFrustum(viewMatrix, projectionMatrix);
    }

    static const XMFLOAT4 AABB[] = {
        {-0.5, -0.5, -0.5, 1.0},
        {0.5,  0.5, 0.5, 1.0}
    };

    cubesIndexies.clear();
    for (int i = 0; i < MAX_CUBES; i++) {
        XMFLOAT4 min, max;

        XMStoreFloat4(&min, XMVector4Transform(XMLoadFloat4(&AABB[0]), geomBufferInst[i].worldMatrix));
        XMStoreFloat4(&max, XMVector4Transform(XMLoadFloat4(&AABB[1]), geomBufferInst[i].worldMatrix));

        if (isInFrustum(max.x, max.y, max.z, min.x, min.y, min.z))
            cubesIndexies.push_back(i);
    }

    D3D11_MAPPED_SUBRESOURCE subresource;
    HRESULT hr = context->Map(g_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    if (FAILED(hr))
        return FAILED(hr);

    CubeSceneMatrixBuffer& sceneBuffer = *reinterpret_cast<CubeSceneMatrixBuffer*>(subresource.pData);
    sceneBuffer.viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    for (int i = 0; i < cubesIndexies.size(); i++)
        sceneBuffer.indexBuffer[i] = XMINT4(cubesIndexies[i], 0, 0, 0);

    context->Unmap(g_pSceneMatrixBuffer, 0);

    hr = context->Map(g_LightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    if (FAILED(hr))
        return FAILED(hr);

    LightableCB& lightBuffer = *reinterpret_cast<LightableCB*>(subresource.pData);
    lightBuffer.cameraPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
    lightBuffer.ambientColor = XMFLOAT4(0.9f, 0.9f, 0.4f, 1.0f);
    auto& lightColors = lights.getColors();
    auto& lightPos = lights.getPositions();
    lightBuffer.lightCount = XMINT4(int(lightColors.size()), 1, 0, 0);

    for (int i = 0; i < lightColors.size(); i++) {
        lightBuffer.lightPos[i] = XMFLOAT4(lightPos[i].x, lightPos[i].y, lightPos[i].z, 1.0f);
        lightBuffer.lightColor[i] = XMFLOAT4(lightColors[i].x, lightColors[i].y, lightColors[i].z, 1.0f);
    }
    context->Unmap(g_LightConstantBuffer, 0);

    return S_OK;
}
