#pragma once

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

#define MOVEMENT_DOWNSHIFTING 300.f

class Camera {
public:
  HRESULT init();
  void frame();
  void realese() {};
  void move(float dx, float dy, float wheel);
  void getViewMatrix(XMMATRIX& viewMatrix);

private:
  XMMATRIX viewMatrix;
  XMFLOAT3 pointOfInterest;

  float distanceToPoint;
  float phi;
  float theta;
};