#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include <ctime>

#include "camera.h"
#include "input.h"


struct SimpleVertex
{
  float x, y, z;
  COLORREF color;
};
struct WorldMatrixBuffer {
  XMMATRIX worldMatrix;
};
struct SceneMatrixBuffer {
  XMMATRIX viewProjectionMatrix;
};

class Renderer {
public:
  static Renderer& getInstance();
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;

  HRESULT init(const HWND& g_hWnd, const HINSTANCE& g_hInstance, UINT screenWidth, UINT screenHeight);
  bool Frame();
  void render();
  void deviceCleanup();
  void resizeWindow(const HWND& g_hWnd);

private:
  HRESULT initDevice(const HWND& g_hWnd);
  HRESULT compileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
  void handleInput();
  Renderer() = default;

  D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
  D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
  ID3D11Device*           g_pd3dDevice = nullptr;
  ID3D11Device1*          g_pd3dDevice1 = nullptr;
  ID3D11DeviceContext*    g_pImmediateContext = nullptr;
  ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
  IDXGISwapChain*         g_pSwapChain = nullptr;
  IDXGISwapChain1*        g_pSwapChain1 = nullptr;
  ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

  ID3D11VertexShader* g_pVertexShader = nullptr;
  ID3D11PixelShader* g_pPixelShader = nullptr;
  ID3D11InputLayout* g_pVertexLayout = nullptr;

  ID3D11Buffer* g_pVertexBuffer = nullptr;
  ID3D11Buffer* g_pIndexBuffer = nullptr;
  ID3D11Buffer* g_pWorldMatrixBuffer = nullptr;
  ID3D11Buffer* g_pSceneMatrixBuffer = nullptr;
  ID3D11RasterizerState* g_pRasterizerState = nullptr;

  std::clock_t init_time;
  Camera camera;
  Input input;
  float angle_velocity = 3.1415926f;
};