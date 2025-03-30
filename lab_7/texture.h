#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <vector>

#include "DDSTextureLoader.h"

class Texture {
public:
	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
	HRESULT initEx(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
	HRESULT initArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<const wchar_t*>& filenames);
	void realize();
	ID3D11ShaderResourceView* getTexture();
private:
	ID3D11ShaderResourceView* g_pTextureView = nullptr;
};