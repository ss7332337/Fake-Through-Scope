#include "RTT.h"
#include <DirectXMath.h>
#pragma once

#ifndef _MACRO_H
#define _MACRO_H
#define HR(X) {if(FAILED(X)) { MessageBoxW(0,L"Create Failed",0,0); return false;}}
#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } }

#endif // !_MACRO_H

ID3D11Buffer* vertex_buffer = nullptr;
ID3D11Buffer* index_buffer = nullptr;

//ID3D11DepthStencilState* md3dDisableDepthStencilState;

RenderModelToTextureClass::RenderModelToTextureClass()
{
	mRenderTargetTexture = NULL;
	mRenderTargetView = NULL;
	mShaderResourceView = NULL;
}

RenderModelToTextureClass::RenderModelToTextureClass(const RenderModelToTextureClass& other)
{
}

RenderModelToTextureClass::~RenderModelToTextureClass()
{
}

bool RenderModelToTextureClass::Initialize(ID3D11Device* d3dDevice, int TextureWidth, int TexureHeight)
{
	//第一,填充2D纹理形容结构体,并创建2D渲染纹理
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = TextureWidth;
	textureDesc.Height = TexureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HR(d3dDevice->CreateTexture2D(&textureDesc, NULL, &mRenderTargetTexture));

	//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	HR(d3dDevice->CreateRenderTargetView(mRenderTargetTexture, &renderTargetViewDesc, &mRenderTargetView));


	//第三,填充着色器资源视图形容体,并进行创建着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HR(d3dDevice->CreateShaderResourceView(mRenderTargetTexture, &shaderResourceViewDesc, &mShaderResourceView));


	D3D11_DEPTH_STENCIL_DESC DisableDepthDESC;
	ZeroMemory(&DisableDepthDESC, sizeof(DisableDepthDESC));
	DisableDepthDESC.DepthEnable = false;
	DisableDepthDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DisableDepthDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DisableDepthDESC.StencilEnable = true;
	DisableDepthDESC.StencilReadMask = 0xff;
	DisableDepthDESC.StencilWriteMask = 0xff;
	//前面设定
	DisableDepthDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DisableDepthDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定,在光栅化状态剔除背面时这个设定没用,但是依然要设定,不然无法创建深度(模板)状态
	DisableDepthDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DisableDepthDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(d3dDevice->CreateDepthStencilState(&DisableDepthDESC, &md3dDepthStencilState));

	return true;
}


struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 color;
};


bool RenderModelToTextureClass::InitializeBuffer(ID3D11Device* d3dDevice)
{
	Vertex* vertexs = NULL;
	WORD* indices = NULL;  //一个字为两个字节

	int mVertexCount = 6;
	int mIndexCount = 6;

	//创建顶点数组
	vertexs = new Vertex[mVertexCount];
	if (!vertexs)
		return false;

	//创建索引数组
	indices = new WORD[mIndexCount];
	if (!indices)
		return false;

	//初始化顶点数组为0
	memset(vertexs, 0, sizeof(Vertex) * mVertexCount);

	//赋予索引数组数据
	//注意用左手定则判定是不是背面
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	//第一,填充(顶点)缓存形容结构体和子资源数据结构体,并创建顶点缓存(这里用的是动态缓存)
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexs;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HR(d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertex_buffer));

	//第二,填充(索引)缓存形容结构体和子资源数据结构体,并创建索引缓存
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	HR(d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer));

	//释放顶点数组和索引数组(这时数据已经载入缓存,不需要这些数组了)
	delete[] vertexs;
	vertexs = NULL;
	delete[] indices;
	indices = NULL;

	return true;
}

void RenderModelToTextureClass::ShutDown()
{
	ReleaseCOM(mRenderTargetTexture);
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mShaderResourceView);
}

//void RenderModelToTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
//{
//	//绑定渲染目标视图和深度模板视图到输出渲染管线,使渲染目的地变为一张纹理资源mShaderResourceView
//
//	deviceContext->OMSetRenderTargets(1, &mRenderTargetView,md3dDepthStencilView);
//}




void RenderModelToTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	//绑定渲染目标视图和深度模板视图到输出渲染管线,使渲染目的地变为一张纹理资源mShaderResourceView

	deviceContext->OMSetRenderTargets(1, &mRenderTargetView, depthStencilView);
}

void RenderModelToTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* const* ppRenderTargetViews)
{
	//绑定渲染目标视图和深度模板视图到输出渲染管线,使渲染目的地变为一张纹理资源mShaderResourceView

	deviceContext->OMSetRenderTargets(1, ppRenderTargetViews, depthStencilView);
}

void RenderModelToTextureClass::TurnOffZBuffer(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(md3dDepthStencilState, 2);
}

//void RenderModelToTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
//{
//	//设置清除缓存为的颜色
//	float color[4];
//	color[0] = red;
//	color[1] = green;
//	color[2] = blue;
//	color[3] = alpha;
//
//	//清除背后缓存
//	deviceContext->ClearRenderTargetView(mRenderTargetView, color);
//
//	//清除深度缓存和模板缓存
//	deviceContext->ClearDepthStencilView(md3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//}

void RenderModelToTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha)
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除背后缓存
	deviceContext->ClearRenderTargetView(mRenderTargetView, color);
	
	//清除深度缓存和模板缓存
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// 将“被渲染模型到纹理的纹理”作为ShaderResourceView资源返回，这个资源将会跟其它的ShaderResourceView资源一样被送入Shader里计算.
ID3D11ShaderResourceView* RenderModelToTextureClass::GetShaderResourceView()
{
	return mShaderResourceView;
}

ID3D11Texture2D* RenderModelToTextureClass::GetRenderTargetTexture()
{
	return mRenderTargetTexture;
}
