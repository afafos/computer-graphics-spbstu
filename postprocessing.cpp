#include "postprocessing.h"

HRESULT Postprocessing::init(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight) {
    HRESULT hr = S_OK;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    hr = D3DReadFileToBlob(L"PostprocessingVertexShader.cso", &vertexShaderBuffer);
    hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"PostprocessingVertexShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    hr = D3DReadFileToBlob(L"PostprocessingPixelShader.cso", &pixelShaderBuffer);
    hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader);
    if (FAILED(hr)) {
        MessageBox(nullptr, L"PostprocessingPixelShader.cso not found.", L"Error", MB_OK);
        return hr;
    }

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

    hr = device->CreateSamplerState(&samplerDesc, &g_pSamplerState);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(PostprocessingCB);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    PostprocessingCB postCB;
    postCB.params = XMINT4(0, 0, 0, 0);

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &postCB;
    data.SysMemPitch = sizeof(PostprocessingCB);
    data.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&desc, &data, &g_pPostprocessingCB);

    return hr;
}

void Postprocessing::realize() {
    if (g_pSamplerState) g_pSamplerState->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPostprocessingCB) g_pPostprocessingCB->Release();
}


void Postprocessing::render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport) {
    context->OMSetRenderTargets(1, &renderTarget, nullptr);
    context->RSSetViewports(1, &viewport);

    context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(g_pVertexShader, nullptr, 0);
    context->PSSetShader(g_pPixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &g_pPostprocessingCB);
    context->PSSetShaderResources(0, 1, &sourceTexture);
    context->PSSetSamplers(0, 1, &g_pSamplerState);

    context->Draw(3, 0);

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    context->PSSetShaderResources(0, 1, nullsrv);
}

bool Postprocessing::frame(ID3D11DeviceContext* context, bool usePosteffect) {
    PostprocessingCB postCB;
    postCB.params = XMINT4(usePosteffect, m_screenWidth, m_screenHeight, 0);

    context->UpdateSubresource(g_pPostprocessingCB, 0, nullptr, &postCB, 0, 0);
    return true;
}

void Postprocessing::resize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}