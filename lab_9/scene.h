#pragma once

#include <windows.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>

#include "skybox.h"
#include "cube.h"
#include "plane.h"
#include "timer.h"
#include "texture.h"
#include "light.h"

using namespace DirectX;

class Scene {
public:
    HRESULT init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight);
    void realize();
    void resize(int screenWidth, int screenHeight);
    void render(ID3D11DeviceContext* context);
    bool frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, bool fixFrustumCulling);
    int getRenderedCount() { return cube.getRenderedCubesCount(); };
private:
    bool framePlanes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

    Cube cube;
    Plane planes;
    Skybox skybox;
    Light lights;

    float angle_velocity = XM_PIDIV2;
};