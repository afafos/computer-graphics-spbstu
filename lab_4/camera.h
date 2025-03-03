#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

constexpr float movement_downshifting = 300.f;

class Camera {
public:
	HRESULT init();
	void realese() {};
	void Frame();
	void getViewMatrix(XMMATRIX& viewMatrix);
	XMFLOAT3 getPos();
	void move(float dx, float dy);
	void updateDistance(float wheel);

private:
	XMMATRIX viewMatrix;
	XMFLOAT3 pointOfInterest;

	float distanceToPoint;
	float phi;
	float theta;
};