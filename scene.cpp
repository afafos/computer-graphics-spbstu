#include "Scene.h"

HRESULT Scene::init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight) {
    HRESULT hr = cubes.init(device, context, screenWidth, screenHeight, 2, L"./cat.dds", L"./texture_norm.dds", 64.f);
    if (FAILED(hr))
        return hr;

    std::vector<XMFLOAT4> colors = {
      XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f),
      XMFLOAT4(0.5f, 1.f, 0.f, 1.f),
    };

    hr = planes.init(device, context, screenWidth, screenHeight, 2, colors);
    if (FAILED(hr))
        return hr;

    hr = skybox.init(device, context, screenWidth, screenHeight);

    lights = std::vector<Light>(3);
    hr = lights[0].init(device, context, screenWidth, screenHeight, XMFLOAT4(1.f, 1.f, 1.0f, 1.f), XMFLOAT4(-1.f, 0.f, 0.f, 1.f));
    hr = lights[1].init(device, context, screenWidth, screenHeight, XMFLOAT4(1.f, 1.f, 1.0f, 1.f), XMFLOAT4(0.f, -1.f, 0.f, 1.f));
    hr = lights[2].init(device, context, screenWidth, screenHeight, XMFLOAT4(1.f, 1.f, 0.0f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f));

    return hr;
}

void Scene::realize() {
    cubes.realize();
    planes.realize();
    skybox.realize();
    for (auto& light : lights)
        light.realize();
}

void Scene::render(ID3D11DeviceContext* context) {
    cubes.render(context);
    skybox.render(context);
    for (auto& light : lights)
        light.render(context);
    planes.render(context);
}

bool Scene::frameCubes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    auto duration = Timer::GetInstance().Clock();
    std::vector<XMMATRIX> worldMatricies = std::vector<XMMATRIX>(2);

    worldMatricies[0] = XMMatrixRotationY((float)duration * angle_velocity);
    worldMatricies[1] = XMMatrixTranslation((float)sin(duration) * 3.0f, 0.0f, (float)cos(duration) * 2.0f) * XMMatrixRotationX((float)sin(duration) * 2.f);
    bool failed = cubes.frame(context, worldMatricies, viewMatrix, projectionMatrix, cameraPos, lights);

    return failed;
}

bool Scene::framePlanes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    auto duration = Timer::GetInstance().Clock();
    std::vector<XMMATRIX> worldMatricies = std::vector<XMMATRIX>(2);

    worldMatricies[0] = XMMatrixTranslation(1.1f, 1.f, (float)(sin(duration * 3) * 2.0));
    worldMatricies[1] = XMMatrixTranslation(1.2f, (float)(sin(duration * 2) * 2.0), 1.f);

    bool failed = planes.frame(context, worldMatricies, viewMatrix, projectionMatrix, cameraPos, lights);

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

    for (auto& light : lights)
        light.frame(context, viewMatrix, projectionMatrix, cameraPos);

    return failed;
}

void Scene::resize(int screenWidth, int screenHeight) {
    cubes.resize(screenWidth, screenHeight);
    planes.resize(screenWidth, screenHeight);
    skybox.resize(screenWidth, screenHeight);
    for (auto& light : lights)
        light.resize(screenWidth, screenHeight);
};
