#pragma once

#include <d3d11.h>
#include <stdio.h>

#include "DDSTextureLoader.h"

class Texture {
public:
	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
	HRESULT InitEx(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);

	void Release();

	ID3D11ShaderResourceView* GetTexture();
private:
	ID3D11ShaderResourceView* g_pTextureView = nullptr;
};