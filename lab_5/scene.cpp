#include "scene.h"

HRESULT Scene::init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight) {
    std::vector<XMFLOAT4> colors = {
      XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f),
      XMFLOAT4(0.5f, 1.f, 0.f, 1.f),
    };

    HRESULT hr = cubes.init(device, context, screenWidth, screenHeight, 2);
    if (FAILED(hr))
        return hr;

    hr = planes.init(device, context, screenWidth, screenHeight, 2, colors);
    if (FAILED(hr))
        return hr;

    hr = skybox.init(device, context, screenWidth, screenHeight);

    return hr;
}

void Scene::realize() {
    cubes.realize();
    planes.realize();
    skybox.realize();
}

void Scene::render(ID3D11DeviceContext* context) {
    cubes.render(context);
    skybox.render(context);
    planes.render(context);
}

bool Scene::frameCubes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    auto duration = Timer::GetInstance().Clock();
    std::vector<XMMATRIX> worldMatricies = std::vector<XMMATRIX>(2);

    worldMatricies[0] = XMMatrixRotationY((float)duration * angle_velocity);
    worldMatricies[1] = XMMatrixTranslation((float)tan(duration) * 3.0f, 1.0f, (float)cos(duration) * 2.0f) * XMMatrixRotationX((float)sin(duration) * 2.f);
    bool failed = cubes.frame(context, worldMatricies, viewMatrix, projectionMatrix, cameraPos);

    return failed;
}

bool Scene::framePlanes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    auto duration = Timer::GetInstance().Clock();
    std::vector<XMMATRIX> worldMatricies = std::vector<XMMATRIX>(2);

    worldMatricies[0] = XMMatrixTranslation(1.f, 0.5f, (float)(sin(duration * 3) * 5.0));
    worldMatricies[1] = XMMatrixTranslation(1.f, (float)(sin(duration * 0.5) * 2.0), 1.f);


    bool failed = planes.frame(context, worldMatricies, viewMatrix, projectionMatrix, cameraPos);

    return failed;
}

bool Scene::frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    bool failed = frameCubes(context, viewMatrix, projectionMatrix, cameraPos);
    if (failed)
        return false;

    failed = framePlanes(context, viewMatrix, projectionMatrix, cameraPos);
    if (failed)
        return false;

    failed = skybox.frame(context, viewMatrix, projectionMatrix, cameraPos);

    return failed;
}

void Scene::resize(int screenWidth, int screenHeight) {
    cubes.resize(screenWidth, screenHeight);
    planes.resize(screenWidth, screenHeight);
    skybox.resize(screenWidth, screenHeight);
};