#include "Renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

using namespace DirectX;

Renderer& Renderer::getInstance() {
    static Renderer rendererInstance;
    return rendererInstance;
}

HRESULT Renderer::initDevice(const HWND& g_hWnd) {
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
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
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));
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

        hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
        }

        dxgiFactory2->Release();
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
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    }

    dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    hr = initBackBuffer();
    if (FAILED(hr))
        return hr;

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    scene.init(g_pd3dDevice, g_pImmediateContext, width, height);

    return S_OK;
}

HRESULT Renderer::initBackBuffer() {
    ID3D11Texture2D* pBackBuffer = NULL;
    HRESULT hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    if (pBackBuffer) pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    if (g_pDepthBuffer) g_pDepthBuffer->Release();
    if (g_pDepthBufferDSV) g_pDepthBufferDSV->Release();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Height = m_height;
    desc.Width = m_width;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    hr = g_pd3dDevice->CreateTexture2D(&desc, NULL, &g_pDepthBuffer);
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthBuffer, NULL, &g_pDepthBufferDSV);
    return hr;
}

HRESULT Renderer::init(const HWND& g_hWnd, const HINSTANCE& g_hInstance, UINT screenWidth, UINT screenHeight) {
    resize(screenWidth, screenHeight);

    m_fixFrustumCulling = false;
    m_usePosteffect = false;

    m_rbPressed = false;
    m_prevMouseX = 0;
    m_prevMouseY = 0;

    HRESULT hr = camera.init();
    if (FAILED(hr))
        return hr;

    hr = initDevice(g_hWnd);
    if (FAILED(hr))
        return hr;

    hr = renderTexture.init(g_pd3dDevice, screenWidth, screenHeight);
    if (FAILED(hr))
        return hr;

    hr = postprocessing.init(g_pd3dDevice, g_hWnd, screenWidth, screenHeight);
    if (FAILED(hr))
        return hr;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pImmediateContext);
    
    return S_OK;
}

void Renderer::mouseMoved(int x, int y) {
    if (m_rbPressed) {
        float dx = (float)(x - m_prevMouseX) * angle_velocity;
        float dy = (float)(y - m_prevMouseY) * angle_velocity;

        m_prevMouseX = x;
        m_prevMouseY = y;
        camera.move(dx, dy);
    }
}

void Renderer::mouseRBPressed(bool pressed, int x, int y) {
    m_rbPressed = pressed;

    if (m_rbPressed) {
        m_prevMouseX = x;
        m_prevMouseY = y;
    }
}

void Renderer::mouseWheel(int wheel) {
    camera.updateDistance((float)wheel);
}

bool Renderer::frame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("ImGui");
#ifdef _DEBUG
        ImGui::Checkbox("Fix Frustum Culling", &m_fixFrustumCulling);
#endif
        ImGui::Checkbox("Sobel filter", &m_usePosteffect);
        ImGui::End();
    }
    postprocessing.frame(g_pImmediateContext, m_usePosteffect);
    camera.frame();

    XMMATRIX mView;
    camera.getViewMatrix(mView);


    XMMATRIX mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)m_width / (FLOAT)m_height, 100.0f, 0.01f);
    HRESULT hr = scene.frame(g_pImmediateContext, mView, mProjection, camera.getPos(), m_fixFrustumCulling);
    if (FAILED(hr))
        return FAILED(hr);

    return SUCCEEDED(hr);
}

void Renderer::render() {
    g_pImmediateContext->ClearState();

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)m_width;
    viewport.Height = (FLOAT)m_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    g_pImmediateContext->RSSetViewports(1, &viewport);

    D3D11_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = m_width;
    rect.bottom = m_height;
    g_pImmediateContext->RSSetScissorRects(1, &rect);

    renderTexture.setRenderTarget(g_pImmediateContext, g_pDepthBufferDSV);
    renderTexture.clearRenderTarget(g_pImmediateContext, g_pDepthBufferDSV, 0.0f, 0.0f, 0.0f, 1.0f);

    scene.render(g_pImmediateContext);

    ID3D11RenderTargetView* views[] = { g_pRenderTargetView };
    g_pImmediateContext->OMSetRenderTargets(1, views, g_pDepthBufferDSV);

    static const FLOAT BackColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, BackColor);
    g_pImmediateContext->ClearDepthStencilView(g_pDepthBufferDSV, D3D11_CLEAR_DEPTH, 0.0f, 0);

    postprocessing.render(g_pImmediateContext, renderTexture.getShaderResourceView(), g_pRenderTargetView, viewport);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(0, 0);
}

void Renderer::resize(UINT screenWidth, UINT screenHeight) {
    m_width = screenWidth;
    m_height = screenHeight;
}

void Renderer::deviceCleanup() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    camera.realize();
    scene.realize();
    renderTexture.realize();
    postprocessing.realize();
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pDepthBuffer) g_pDepthBuffer->Release();
    if (g_pDepthBufferDSV) g_pDepthBufferDSV->Release();
    if (g_pSwapChain1) g_pSwapChain1->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext1) g_pImmediateContext1->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice1) g_pd3dDevice1->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}

void Renderer::resizeWindow(const HWND& g_hWnd) {
    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    if (g_pSwapChain && (width != m_width || height != m_height)) {

        if (g_pRenderTargetView) g_pRenderTargetView->Release();

        HRESULT hr = g_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        assert(SUCCEEDED(hr));
        if (SUCCEEDED(hr)) {
            resize(width, height);

            hr = initBackBuffer();
            resize(width, height);
            scene.resize(width, height);
            renderTexture.resize(g_pd3dDevice, width, height);
            postprocessing.resize(width, height);
        }
    }
}