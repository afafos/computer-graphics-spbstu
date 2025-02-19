#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


#include <dinput.h>
#include <directxmath.h>

using namespace DirectX;

class Input {
public:
  HRESULT init(const HINSTANCE & g_hInstance, const HWND &hwnd, UINT screenWidth, UINT screenHeight);
  bool frame();
  void realese();
  XMFLOAT3 isMouseUsed();
  float isSignPressed();
  void resize(UINT screenWidth, UINT screenHeight);
  UINT getWidth() { return wWidth; }
  UINT getHeight() { return wHeight; }

private:
  bool readKeyboard();
  bool readMouse();

  IDirectInput8* directInput = nullptr;
  IDirectInputDevice8* keyboard = nullptr;
  IDirectInputDevice8* mouse = nullptr;

  unsigned char keyboardState[256];
  DIMOUSESTATE mouseState = {};
  UINT wWidth = 0, wHeight = 0;
};
