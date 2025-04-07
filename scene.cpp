#include "scene.h"

HRESULT Scene::init(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight) {
    std::vector<XMFLOAT4> cubePositions = std::vector<XMFLOAT4>(MAX_CUBES);
    for (int i = 0; i < MAX_CUBES; i++) {
        cubePositions[i] = XMFLOAT4(
            (rand() / (float)(RAND_MAX + 1) * SCENE_SIZE - SCENE_SIZE / 2.f),
            (rand() / (float)(RAND_MAX + 1) * SCENE_SIZE - SCENE_SIZE / 2.f),
            (rand() / (float)(RAND_MAX + 1) * SCENE_SIZE - SCENE_SIZE / 2.f), 1.f);
    }
    HRESULT hr = cube.init(device, context, screenWidth, screenHeight, { L"./cat.dds", L"./cat.dds"}, L"./texture_norm.dds", 32.f, cubePositions);
    if (FAILED(hr))
        return hr;

    std::vector<XMFLOAT4> planeColors = {
      XMFLOAT4(1.f, 0.f, 0.f, 0.5f),
      XMFLOAT4(0.f, 1.f, 0.f, 0.5f),
      XMFLOAT4(0.f, 0.f, 1.f, 0.5f),
    };

    hr = planes.init(device, context, screenWidth, screenHeight, 3, planeColors);
    if (FAILED(hr))
        return hr;

    hr = skybox.init(device, context, screenWidth, screenHeight);

    std::vector<XMFLOAT4> colors = std::vector<XMFLOAT4>(MAX_LIGHTS);
    std::vector<XMFLOAT4> positions = std::vector<XMFLOAT4>(MAX_LIGHTS);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        colors[i] = XMFLOAT4(
            (float)(0.5f + rand() / (float)(RAND_MAX + 1) * 0.5f),
            (float)(0.5f + rand() / (float)(RAND_MAX + 1) * 0.5f),
            (float)(0.5f + rand() / (float)(RAND_MAX + 1) * 0.5f), 1.f);
        positions[i] = XMFLOAT4(
            (rand() / (float)(RAND_MAX + 1) * SCENE_SIZE - SCENE_SIZE / 2.f),
            (rand() / (float)(RAND_MAX + 1) * SCENE_SIZE - SCENE_SIZE / 2.f),
            (rand() / (float)(RAND_MAX + 1) * SCENE_SIZE - SCENE_SIZE / 2.f), 1.f);
    }
    hr = lights.init(device, context, screenWidth, screenHeight, colors, positions);

    return hr;
}

void Scene::realize() {
    cube.realize();
    planes.realize();
    skybox.realize();
    lights.realize();
}

void Scene::render(ID3D11DeviceContext* context) {
    cube.render(context);
    lights.render(context);
    skybox.render(context);
    planes.render(context);
}

bool Scene::framePlanes(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {
    auto duration = Timer::GetInstance().Clock();
    std::vector<XMMATRIX> worldMatricies = std::vector<XMMATRIX>(3);

    worldMatricies[0] = XMMatrixTranslation(1.25f, (float)(3.5 * sqrtf(2.0) * cos(duration) / (1.0 + sin(duration) * sin(duration))), (float)(3.5*sqrtf(2.0)*sin(duration)*cos(duration)/(1+ sin(duration)* sin(duration))));
    worldMatricies[1] = XMMatrixTranslation(-1.25f, (float)(sin(duration * 2) * 2.0), (float)(sin(duration * 2) * -2.0));
    worldMatricies[2] = XMMatrixTranslation(1.5f, (float)(sin(duration * 2) * 2.0), (float)(sin(duration * 2) * 2.0));

    bool failed = planes.frame(context, worldMatricies, viewMatrix, projectionMatrix, cameraPos, lights);

    return failed;
}

bool Scene::frame(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, bool fixFrustumCulling) {
    bool failed = cube.frame(context, viewMatrix, projectionMatrix, cameraPos, lights, fixFrustumCulling);
    if (failed)
        return false;

    failed = framePlanes(context, viewMatrix, projectionMatrix, cameraPos);
    if (failed)
        return false;

    failed = skybox.frame(context, viewMatrix, projectionMatrix, cameraPos);
    if (failed)
        return false;

    failed = lights.frame(context, viewMatrix, projectionMatrix, cameraPos);

    return failed;
}

void Scene::resize(int screenWidth, int screenHeight) {
    cube.resize(screenWidth, screenHeight);
    planes.resize(screenWidth, screenHeight);
    skybox.resize(screenWidth, screenHeight);
    lights.resize(screenWidth, screenHeight);
};