#include <windows.h>
#include <xstring>

#include "resource.h"
#include "renderer.h"


#define START_W 1280
#define START_H 720

#define MAX_LOADSTRING 300
WCHAR szTitle[MAX_LOADSTRING];                  


HINSTANCE       g_hInst = nullptr;
HWND            g_hWnd = nullptr;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = nullptr;
  wcex.hIconSm = nullptr;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = L"WindowClass";
  if (!RegisterClassEx(&wcex))
    return E_FAIL;

  g_hInst = hInstance;
  RECT rc = { 0, 0, START_W, START_H };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  g_hWnd = CreateWindow(L"WindowClass", L"Lab_3, AfanasyevAV",
    WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
    nullptr);
  if (!g_hWnd)
    return E_FAIL;

  ShowWindow(g_hWnd, nCmdShow);
  SetForegroundWindow(g_hWnd);
  SetFocus(g_hWnd);

  return S_OK;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  if (FAILED(InitWindow(hInstance, nCmdShow)))
    return 0;

  
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

  std::wstring dir;
  dir.resize(MAX_LOADSTRING + 1);
  GetCurrentDirectory(MAX_LOADSTRING + 1, &dir[0]);
  size_t configPos = dir.find(L"x64");
  if (configPos != std::wstring::npos)
  {
    dir.resize(configPos);
    dir += szTitle;
    SetCurrentDirectory(dir.c_str());
  }

  if (FAILED(Renderer::getInstance().init(g_hWnd, g_hInst, START_W, START_H)))
  {
    Renderer::getInstance().deviceCleanup();
    return 0;
  }

  MSG msg = { 0 };
  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (Renderer::getInstance().Frame())
      Renderer::getInstance().render();
  }

  Renderer::getInstance().deviceCleanup();

  return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message)
  {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    break;

  
  case WM_GETMINMAXINFO:
  {
    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    lpMMI->ptMinTrackSize.x = 256;
    lpMMI->ptMinTrackSize.y = 256;
    break;
  }

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_SIZE:
    Renderer::getInstance().resizeWindow(g_hWnd);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

