#pragma once

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

using namespace DirectX;

class Scene {
public:
    HRESULT init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight);
    void realize();
    void resize(int screenWidth, int screenHeight);
    void render(ID3D11DeviceContext* context);
    bool frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

private:
    bool frameCubes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);
    bool framePlanes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);

    Cube cubes;
    Plane planes;
    Skybox skybox;

    float angle_velocity = XM_PIDIV2;
};