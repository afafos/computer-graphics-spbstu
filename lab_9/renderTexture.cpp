#include "renderTexture.h"

HRESULT RenderTexture::init(ID3D11Device* device, int screenWidth, int screenHeight) {
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = screenWidth;
    textureDesc.Height = screenHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &g_pRenderTargetTexture);
    if (FAILED(hr))
        return hr;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    hr = device->CreateRenderTargetView(g_pRenderTargetTexture, &renderTargetViewDesc, &g_pRenderTargetView);
    if (FAILED(hr))
        return hr;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(g_pRenderTargetTexture, &shaderResourceViewDesc, &g_pShaderResourceView);
    if (FAILED(hr))
        return hr;

    g_viewport.Width = (FLOAT)screenWidth;
    g_viewport.Height = (FLOAT)screenHeight;
    g_viewport.MinDepth = 0.0f;
    g_viewport.MaxDepth = 1.0f;
    g_viewport.TopLeftX = 0;
    g_viewport.TopLeftY = 0;

    return S_OK;
}

void RenderTexture::realize() {
    if (g_pShaderResourceView) g_pShaderResourceView->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pRenderTargetTexture) g_pRenderTargetTexture->Release();

    ZeroMemory(&g_viewport, sizeof(D3D11_VIEWPORT));
}

void RenderTexture::resize(ID3D11Device* device, int width, int height) {
    realize();
    init(device, width, height);
}

void RenderTexture::clearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha) {
    float color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    deviceContext->ClearRenderTargetView(g_pRenderTargetView, color);

    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 0.0f, 0);
}