#include "light.h"

void Light::generateSphere(UINT LatLines, UINT LongLines, std::vector<SimpleVertex>& vertices, std::vector<UINT>& indices) {
    numSphereVertices = ((LatLines - 2) * LongLines) + 2;
    numSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

    float sphereYaw = 0.0f;
    float spherePitch = 0.0f;

    vertices.resize(numSphereVertices);

    XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 1.0f;

    for (UINT i = 0; i < LatLines - 2; i++) {
        spherePitch = (i + 1) * (XM_PI / (LatLines - 1));
        XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
        for (UINT j = 0; j < LongLines; j++) {
            sphereYaw = j * (XM_2PI / (LongLines));
            XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
            currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
            currVertPos = XMVector3Normalize(currVertPos);
            vertices[i * LongLines + j + 1].x = XMVectorGetX(currVertPos);
            vertices[i * LongLines + j + 1].y = XMVectorGetY(currVertPos);
            vertices[i * LongLines + j + 1].z = XMVectorGetZ(currVertPos);
        }
    }

    vertices[numSphereVertices - 1].x = 0.0f;
    vertices[numSphereVertices - 1].y = 0.0f;
    vertices[numSphereVertices - 1].z = -1.0f;

    indices.resize(numSphereFaces * 3);

    UINT k = 0;
    for (UINT i = 0; i < LongLines - 1; i++) {
        indices[k] = 0;
        indices[k + 1] = i + 1;
        indices[k + 2] = i + 2;
        k += 3;
    }

    indices[k] = 0;
    indices[k + 1] = LongLines;
    indices[k + 2] = 1;
    k += 3;

    for (UINT i = 0; i < LatLines - 3; i++) {
        for (UINT j = 0; j < LongLines - 1; j++) {
            indices[k] = i * LongLines + j + 1;
            indices[k + 1] = i * LongLines + j + 2;
            indices[k + 2] = (i + 1) * LongLines + j + 1;

            indices[k + 3] = (i + 1) * LongLines + j + 1;
            indices[k + 4] = i * LongLines + j + 2;
            indices[k + 5] = (i + 1) * LongLines + j + 2;

            k += 6;
        }

        indices[k] = (i * LongLines) + LongLines;
        indices[k + 1] = (i * LongLines) + 1;
        indices[k + 2] = ((i + 1) * LongLines) + LongLines;

        indices[k + 3] = ((i + 1) * LongLines) + LongLines;
        indices[k + 4] = (i * LongLines) + 1;
        indices[k + 5] = ((i + 1) * LongLines) + 1;

        k += 6;
    }

    for (UINT i = 0; i < LongLines - 1; i++) {
        indices[k] = numSphereVertices - 1;
        indices[k + 1] = (numSphereVertices - 1) - (i + 1);
        indices[k + 2] = (numSphereVertices - 1) - (i + 2);
        k += 3;
    }

    indices[k] = numSphereVertices - 1;
    indices[k + 1] = (numSphereVertices - 1) - LongLines;
    indices[k + 2] = numSphereVertices - 2;

    return;
}

HRESULT Light::init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight,
        const std::vector<XMFLOAT4>& colors, const std::vector<XMFLOAT4>& positions) {

    std::vector<SimpleVertex> vertices;
    std::vector<UINT> indices;
    generateSphere(10, 10, vertices, indices);

    this->colors = colors;
    this->positions = positions;
    assert(this->colors.size() == MAX_LIGHTS);
    assert(this->positions.size() == MAX_LIGHTS);

    static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11_BUFFER_DESC descVert = {};
    descVert.ByteWidth = sizeof(SimpleVertex) * numSphereVertices;
    descVert.Usage = D3D11_USAGE_IMMUTABLE;
    descVert.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    descVert.CPUAccessFlags = 0;
    descVert.MiscFlags = 0;
    descVert.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA dataVert;
    ZeroMemory(&dataVert, sizeof(dataVert));
    dataVert.pSysMem = &vertices[0];
    HRESULT hr = device->CreateBuffer(&descVert, &dataVert, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC descInd = {};
    ZeroMemory(&descInd, sizeof(descInd));

    descInd.ByteWidth = sizeof(UINT) * numSphereFaces * 3;
    descInd.Usage = D3D11_USAGE_IMMUTABLE;
    descInd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    descInd.CPUAccessFlags = 0;
    descInd.MiscFlags = 0;
    descInd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA dataInd;
    dataInd.pSysMem = &indices[0];

    hr = device->CreateBuffer(&descInd, &dataInd, &g_pIndexBuffer);
    if (FAILED(hr))
        return hr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;
    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    hr = D3DReadFileToBlob(L"LightVertexShader.cso", &vertexShaderBuffer);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"LightVertexShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader);
    if (FAILED(hr))
        return hr;

    hr = D3DReadFileToBlob(L"LightPixelShader.cso", &pixelShaderBuffer);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"LightPixelShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader);
    if (FAILED(hr))
        return hr;

    int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
    hr = device->CreateInputLayout(InputDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &g_pVertexLayout);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC descWM = {};
    descWM.ByteWidth = (UINT)(sizeof(WorldMatrixBuffer) * colors.size());
    descWM.Usage = D3D11_USAGE_DEFAULT;
    descWM.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descWM.CPUAccessFlags = 0;
    descWM.MiscFlags = 0;
    descWM.StructureByteStride = 0;

    WorldMatrixBuffer lightGeomBuffer[MAX_LIGHTS];
    for (UINT i = 0; i < MAX_LIGHTS; i++) {
        lightGeomBuffer[i].worldMatrix =
            DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) *
            XMMatrixTranslation(
                this->positions[i].x,
                this->positions[i].y,
                this->positions[i].z);
        lightGeomBuffer[i].color = this->colors[i];
    }

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &lightGeomBuffer;
    data.SysMemPitch = sizeof(lightGeomBuffer);
    data.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&descWM, &data, &g_pWorldMatrixBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC descSM = {};
    descSM.ByteWidth = sizeof(SceneMatrixBuffer);
    descSM.Usage = D3D11_USAGE_DYNAMIC;
    descSM.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    descSM.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    descSM.MiscFlags = 0;
    descSM.StructureByteStride = 0;

    hr = device->CreateBuffer(&descSM, nullptr, &g_pSceneMatrixBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_RASTERIZER_DESC descRast = {};
    descRast.AntialiasedLineEnable = false;
    descRast.FillMode = D3D11_FILL_SOLID;
    descRast.CullMode = D3D11_CULL_NONE;
    descRast.DepthBias = 0;
    descRast.DepthBiasClamp = 0.0f;
    descRast.FrontCounterClockwise = false;
    descRast.DepthClipEnable = true;
    descRast.ScissorEnable = false;
    descRast.MultisampleEnable = false;
    descRast.SlopeScaledDepthBias = 0.0f;

    hr = device->CreateRasterizerState(&descRast, &g_pRasterizerState);
    if (FAILED(hr))
        return hr;

    resize(screenWidth, screenHeight);

    return hr;
}

void Light::realize() {
    if (g_pRasterizerState) g_pRasterizerState->Release();
    if (g_pGeomBuffer) g_pGeomBuffer->Release();
    if (g_pWorldMatrixBuffer) g_pWorldMatrixBuffer->Release();
    if (g_pSceneMatrixBuffer) g_pSceneMatrixBuffer->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
}

void Light::render(ID3D11DeviceContext* context) {
    context->RSSetState(g_pRasterizerState);

    context->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    ID3D11Buffer* vertexBuffers[] = { g_pVertexBuffer };
    UINT strides[] = { 12 };
    UINT offsets[] = { 0 };

    context->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
    context->IASetInputLayout(g_pVertexLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(g_pVertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &g_pWorldMatrixBuffer);
    context->VSSetConstantBuffers(1, 1, &g_pSceneMatrixBuffer);
    context->PSSetShader(g_pPixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &g_pWorldMatrixBuffer);

    context->DrawIndexedInstanced(numSphereFaces * 3, (UINT)colors.size(), 0, 0, 0);
}

bool Light::frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    WorldMatrixBuffer lightGeomBuffer[MAX_LIGHTS];
    for (int i = 0; i < MAX_LIGHTS; i++) {
        lightGeomBuffer[i].worldMatrix = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f)
            * XMMatrixTranslation(positions[i].x, positions[i].y, positions[i].z);
        lightGeomBuffer[i].color = colors[i];
    }
    context->UpdateSubresource(g_pWorldMatrixBuffer, 0, nullptr, &lightGeomBuffer, 0, 0);

    D3D11_MAPPED_SUBRESOURCE subresource;
    HRESULT hr = context->Map(g_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    if (FAILED(hr))
        return FAILED(hr);

    SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
    sceneBuffer.viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
    context->Unmap(g_pSceneMatrixBuffer, 0);

    return S_OK;
}
