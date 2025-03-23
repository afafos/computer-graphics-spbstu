#include "texture.h"

using namespace DirectX;

HRESULT Texture::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename) {
    return CreateDDSTextureFromFile(device, filename, nullptr, &g_pTextureView);
}

HRESULT Texture::InitEx(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename) {
    return CreateDDSTextureFromFileEx(device, deviceContext, filename,
        0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE,
        false, nullptr, &g_pTextureView);
}

ID3D11ShaderResourceView* Texture::GetTexture() {
    return g_pTextureView;
};

void Texture::Release() {
    if (g_pTextureView) {
        g_pTextureView->Release();
        g_pTextureView = nullptr;
    }
}