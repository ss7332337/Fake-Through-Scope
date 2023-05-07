#pragma once
#ifndef _RENDER_MODEL_TO_TEXTURE_H
#define _RENDER_MODEL_TO_TEXTURE_H

#include <D3D11.h>
#include <Windows.h>


class RenderModelToTextureClass
{
private:
	ID3D11Texture2D* mRenderTargetTexture;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11ShaderResourceView* mShaderResourceView;
	ID3D11DepthStencilState* md3dDepthStencilState;

	//ID3D11DepthStencilView* md3dDepthStencilView;    //D3D11深度(模板)视图
	//ID3D11Texture2D* md3dDepthStencilBuffer;         //D3D11的“DepthStencil缓存”
	//ID3D11DepthStencilState* md3dDepthStencilState;  //普通的深度(模板)缓存状态
	//D3D11_TEXTURE2D_DESC mdepthStencilDesc;
	//D3D11_DEPTH_STENCIL_DESC DSDESC;

public:
	RenderModelToTextureClass();
	RenderModelToTextureClass(const RenderModelToTextureClass& other);
	~RenderModelToTextureClass();
	bool Initialize(ID3D11Device* d3dDevice, int TextureWidth, int TexureHeight);
	void ShutDown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* const* ppRenderTargetViews);

	void TurnOffZBuffer(ID3D11DeviceContext* deviceContext);

	bool InitializeBuffer(ID3D11Device* d3dDevice);

	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha);
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11Texture2D* GetRenderTargetTexture();
};
#endif  // !_RENDER_3D_MODEL_TO_TEXTURE_H
