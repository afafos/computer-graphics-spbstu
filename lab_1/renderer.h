#pragma once

#include <d3d11_1.h>
#include <directxcolors.h>
#include <ctime>

class Renderer {
public:
    static Renderer& getInstance();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    HRESULT initDevice(const HWND& g_hWnd);
    void render();
    void deviceCleanup();
    void resizeWindow(const HWND& g_hWnd);

private:
    Renderer() = default;

    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11Device1* g_pd3dDevice1 = nullptr;
    ID3D11DeviceContext* g_pImmediateContext = nullptr;
    ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;

    IDXGISwapChain* g_pSwapChain = nullptr;
    IDXGISwapChain1* g_pSwapChain1 = nullptr;

    ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

    // Для анимации цвета: замеряем время создания
    std::clock_t init_time;
};
