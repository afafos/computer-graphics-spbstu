#include "renderer.h"

using namespace DirectX;

Renderer& Renderer::getInstance()
{
    static Renderer rendererInstance;
    return rendererInstance;
}

HRESULT Renderer::initDevice(const HWND& hWnd)
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(
            nullptr,
            g_driverType,
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &g_pd3dDevice,
            &g_featureLevel,
            &g_pImmediateContext
        );

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(
                nullptr,
                g_driverType,
                nullptr,
                createDeviceFlags,
                &featureLevels[1],
                numFeatureLevels - 1,
                D3D11_SDK_VERSION,
                &g_pd3dDevice,
                &g_featureLevel,
                &g_pImmediateContext
            );
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice),
            reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1),
                    reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2),
        reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1),
            reinterpret_cast<void**>(&g_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1),
                reinterpret_cast<void**>(&g_pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; 
        sd.BufferCount = 2;

        hr = dxgiFactory2->CreateSwapChainForHwnd(
            g_pd3dDevice, hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain),
                reinterpret_cast<void**>(&g_pSwapChain));
        }dxgiFactory2->Release();
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    }

    dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr,
        &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    init_time = clock();

    return S_OK;
}

void Renderer::render()
{
    float duration = (1.0f * clock() - init_time) / CLOCKS_PER_SEC;

    float startColor[4] = { 0.3f, 0.1f, 0.8f, 1.0f };

    float clearColor[4] =
    {
        startColor[0] + 0.5f * sinf(1.0f * duration),   // R
        startColor[1] + 0.5f * sinf(2.0f * duration),   // G
        startColor[2] + 0.5f * cosf(1.5f * duration),   // B
        startColor[3]
    };

    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);

    g_pSwapChain->Present(0, 0);
}

void Renderer::deviceCleanup()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain1)       g_pSwapChain1->Release();
    if (g_pSwapChain)        g_pSwapChain->Release();
    if (g_pImmediateContext1)g_pImmediateContext1->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice1)       g_pd3dDevice1->Release();
    if (g_pd3dDevice)        g_pd3dDevice->Release();
}

void Renderer::resizeWindow(const HWND& g_hWnd)
{
    if (g_pSwapChain)
    {
        g_pImmediateContext->OMSetRenderTargets(0, 0, 0);
        if (g_pRenderTargetView)
        {
            g_pRenderTargetView->Release();
            g_pRenderTargetView = nullptr;
        }

        HRESULT hr = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr))
            return;

        ID3D11Texture2D* pBuffer = nullptr;
        hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            (void**)&pBuffer);
        if (FAILED(hr))
            return;

        hr = g_pd3dDevice->CreateRenderTargetView(pBuffer, NULL,
            &g_pRenderTargetView);
        pBuffer->Release();
        if (FAILED(hr))
            return;

        g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

        RECT rc;
        GetClientRect(g_hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        g_pImmediateContext->RSSetViewports(1, &vp);
    }
}