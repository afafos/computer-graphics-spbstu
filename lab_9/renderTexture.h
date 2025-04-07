#pragma once

#include <d3d11.h>

class RenderTexture {
public:
	HRESULT init(ID3D11Device* device, int screenWidth, int screenHeight);
	void realize();
	void resize(ID3D11Device* device, int screenWidth, int screenHeight);
	void setRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) { deviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, depthStencilView); };
	void clearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha);
	ID3D11Texture2D* getRenderTarget() { return g_pRenderTargetTexture; };
	ID3D11RenderTargetView* getRenderTargetView() { return g_pRenderTargetView; };
	ID3D11ShaderResourceView* getShaderResourceView() { return g_pShaderResourceView; };
	D3D11_VIEWPORT getViewPort() { return g_viewport; };

private:
	ID3D11Texture2D* g_pRenderTargetTexture = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
	ID3D11ShaderResourceView* g_pShaderResourceView = nullptr;
	D3D11_VIEWPORT g_viewport;
};