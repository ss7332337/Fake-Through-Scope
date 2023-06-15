#pragma once

#include <iostream>
#include <sstream>
#include <functional>
#include <initializer_list>
#include <stdint.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "FTSData.h"

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }


void SafeWriteBuf(uintptr_t addr, void* data, size_t len);
void SafeWrite64(uintptr_t addr, UInt64 data);
void* GetIATAddr(void* module, const char* searchDllName, const char* searchImportName);

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace Hook
{
	HRESULT CreateShaderFromFile(
		const WCHAR* csoFileNameInOut,
		const WCHAR* hlslFileName,
		LPCSTR entryPoint,
		LPCSTR shaderModel,
		ID3DBlob** ppBlobOut);

	template <typename Func>
	inline void HookImportFunc(const char* dll, const char* nameFunc, Func& oldFn, std::uintptr_t hookFn)
	{
		std::uintptr_t thunkAddress = (std::uintptr_t)GetIATAddr((UINT8*)GetModuleHandle(NULL), dll, nameFunc);
		oldFn = (Func) * (std::uintptr_t*)thunkAddress;
		SafeWrite64(thunkAddress, hookFn);
	}

	inline std::uintptr_t HookFunc(std::uintptr_t* vtbl, int index, std::uintptr_t hookFn, std::uintptr_t* oldFn)
	{
		std::uintptr_t returnAddy = vtbl[index];
		*oldFn = returnAddy;

		SafeWrite64((std::uintptr_t)(vtbl + index), hookFn);

		return returnAddy;
	}
	using namespace DirectX;

	class D3D
	{

		using CreateDeviceAndSwapChain = decltype(&D3D11CreateDeviceAndSwapChain);
		using ClearState = HRESULT(__stdcall*)(ID3D11DeviceContext*);
		using Present = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
		using OMSetRenderTargets = void(WINAPI*)(ID3D11DeviceContext*,
			__in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) UINT NumViews,
			__in_ecount_opt(NumViews) ID3D11RenderTargetView* const* ppRenderTargetViews,
			__in_opt ID3D11DepthStencilView* pDepthStencilView);
		using ClipCur = decltype(&ClipCursor);
		
		using vfn_DeviceContext_ClearDepthStencilView = void(WINAPI*)(ID3D11DeviceContext*,ID3D11DepthStencilView*,UINT,FLOAT,UINT8);
		typedef int(__stdcall* origClearRenderTargetView)(ID3D11DeviceContext* DeviceContext, ID3D11RenderTargetView* TargetView, const FLOAT ColorRGBA[4]);
		typedef HRESULT(__stdcall* D3D11ResizeBuffersHook)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
		typedef void(__stdcall* D3D11PSSetShaderResourcesHook)(ID3D11DeviceContext* , UINT , UINT , ID3D11ShaderResourceView* const* );

		typedef void(__stdcall* D3D11VSSetConstantBuffers)(ID3D11DeviceContext* ,UINT, UINT, ID3D11Buffer* const*);

		using vfn_DeviceContext_DrawIndexed = void(STDMETHODCALLTYPE*)(ID3D11DeviceContext*, UINT, UINT, INT);
		typedef void(__stdcall* tdefDrawIndexed)(ID3D11DeviceContext*, UINT, UINT, INT);
		//typedef void(__stdcall* vfn_DeviceContext_DrawIndexed)(ID3D11DeviceContext*, UINT, UINT, INT);


	public:
		static D3D& instance()
		{
			static D3D instance;
			return instance;
		}

		static [[nodiscard]] bool Register() noexcept;
		/*bool GetD3D11VMT();
		HRESULT WINAPI HookFuncSwapChainPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
		bool HookPresent();*/

	public:

		__declspec(align(16)) struct ConstBufferData
		{
			float width;
			float height;
		};

		__declspec(align(16)) struct VSConstantData
		{
			XMFLOAT3 eyeDirection = {0,0,0};
			float MovePercentage1 = 0;
			XMFLOAT3 eyeDirectionLerp = { 0, 0, 0 };
			float padding12 = 0;
			XMFLOAT3 CurrWeaponPos = { 0, 0, 0 };
			float padding14 = 0;
			XMFLOAT3 CurrRootPos = { 0, 0, 0 };
			float padding15 = 0;
			XMFLOAT4X4 CameraRotation = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		};

		__declspec(align(16)) struct VSOutConstantData
		{
			XMFLOAT4X4 testingMat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			XMFLOAT4X4 ftsLocalRotation = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			XMFLOAT4X4 ftsWorldRotation = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			XMFLOAT4X4 CameraRotation = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		};

		struct ScopeEffectShaderData
		{
			float camDepth = 1;
			float GameFov = 90;
			float ScopeEffect_Zoom = 1.5F;
			float parallax_Radius = 2.0F ;

			float parallax_relativeFogRadius = 8.0F;
			float parallax_scopeSwayAmount = 2.0F;
			float parallax_maxTravel = 16.0F;
			float ReticleSize = 8;

			float nvIntensity = 3;
			float BaseWeaponPos = 0;
			float MovePercentage = 0;
			int EnableZMove = 0;

			int isCircle = 1;
			int EnableNV = 0;
			int EnableMerge = 0;
			float padding = 0;

			XMFLOAT2 ScopeEffect_Size = { 0, 0 };
			XMFLOAT2 ScopeEffect_OriPositionOffset = { 0, 0 };
			XMFLOAT2 ScopeEffect_OriSize = { 0, 0 };
			XMFLOAT2 ScopeEffect_Offset = { 0, 0 };

			XMFLOAT3 eyeDirection = {0,0,0};
			float padding1 = 0;
			XMFLOAT3 eyeDirectionLerp = {0,0,0};
			float padding2 = 0;
			XMFLOAT3 eyeTranslationLerp = { 0, 0, 0 };
			float padding3 = 0;
			XMFLOAT3 CurrWeaponPos = { 0, 0, 0 };
			float padding4 = 0;
			XMFLOAT3 CurrRootPos = { 0, 0, 0 };
			float padding5 = 0;
			XMFLOAT4X4 CameraRotation = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			
		};

		struct GameConstBuffer
		{
			RE::NiPoint3 virDir;
			RE::NiPoint3 lastVirDir;
			RE::NiPoint3 VirDirLerp;
			RE::NiPoint3 VirTransLerp;
			RE::NiPoint3 weaponPos;
			RE::NiPoint3 rootPos;
			RE::NiMatrix3 camMat;
			RE::NiMatrix3 ftsLocalMat;
			RE::NiMatrix3 ftsWorldMat;
			float deltaZoom;
		};

		void EnableRender(bool flag) { isEnableRender = flag; }
		void Render();
		bool InitEffect();
		bool InitResource();
		void InitInputLayout();
		void OnResize();
		void UpdateScene();
		void ShowMenu(bool);
		void EnableCursor(bool enable);
		void RenderImGui();
		bool GetIsShow() { return isShow; };
		void CreateBlender();
		void CreateConstantBuffer();
		void QueryChangeReticleTexture();
		void ResetZoomDelta();
		void CreateRenderTarget();
		void ResetUIData(ScopeData::FTSData* currData);
		void GrabScreen();
		void RenderToReticleTexture();
		void RenderToReticleTextureNew(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		static void InstallDrawIndexedHook(ID3D11Device* device, ID3D11DeviceContext* context);


	private:
		D3D() {}
		~D3D() {}

		D3D(const D3D&) = delete;
		D3D(D3D&&) = delete;

		D3D& operator=(const D3D&) = delete;
		D3D& operator=(D3D&&) = delete;

		static void __stdcall DrawIndexedHook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		//static HRESULT __stdcall ResizeBuffersHook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
		//static void __stdcall SetShaderResourcesHook(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews);
		//static HRESULT __stdcall ClearStateHook(ID3D11DeviceContext*);
		//static void __stdcall ClearRenderTargetViewHook(ID3D11DeviceContext* DeviceContext, ID3D11RenderTargetView* TargetView, const FLOAT ColorRGBA[4]);
		static HRESULT __stdcall PresentHook(IDXGISwapChain*, UINT, UINT);
		static HRESULT __stdcall D3D11CreateDeviceAndSwapChainHook(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
		static LRESULT __stdcall WndProcHandler(HWND, UINT, WPARAM, LPARAM);
		/*static void WINAPI HookFuncDeviceContext_OMSetRenderTargets(ID3D11DeviceContext* pThis,
			__in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) UINT NumViews,
			__in_ecount_opt(NumViews) ID3D11RenderTargetView* const* ppRenderTargetViews,
			__in_opt ID3D11DepthStencilView* pDepthStencilView);*/
		//static void WINAPI ClearDepthStencilViewHook(ID3D11DeviceContext* pThis, _In_ ID3D11DepthStencilView* pDepthStencilView, _In_ UINT ClearFlags, _In_ FLOAT Depth, _In_ UINT8 Stencil);
		static BOOL __stdcall ClipCursorHook(RECT*);
		static void __stdcall vsSetConstantBuffers(ID3D11DeviceContext* pContext, UINT, UINT, ID3D11Buffer* const*);

		
		//static BOOL __stdcall ClipCursorHook(RECT*);
		//static void EnableCursor(bool enable);
		struct ImageSpaceEffectTemporalAA_Render;
		struct ImageSpaceEffectTemporalAA_IsActive;
		struct ImageSpaceEffectBokehDepthOfField_Render;
		struct ImageSpaceEffectBokehDepthOfField_IsActive;

		struct OldFuncs
		{
			CreateDeviceAndSwapChain d3dCreateDevice;
			ClearState d3dClear;
			Present d3dPresent;
			ClipCur clipCursor;
			WNDPROC wndProc;
			OMSetRenderTargets omSetRenderTargets;
			vfn_DeviceContext_ClearDepthStencilView clearDepthStencilView;
			D3D11ResizeBuffersHook resizeBuffersHook;
			D3D11PSSetShaderResourcesHook setShaderResourcesHook;
			origClearRenderTargetView clearRenderTargetView;
			D3D11VSSetConstantBuffers vsSetConstantBuffers;

			tdefDrawIndexed drawIndexed;
			vfn_DeviceContext_DrawIndexed drawIndexedVFN;
		};
		/*void** g_pDeviceVMT = nullptr;
		void** g_pSwapchainVMT = nullptr;
		void** g_pDeviceContextVMT = nullptr;*/

	public:
		void SetNVG(int);
		void SetGameConstData(GameConstBuffer);
		void SetScopeEffect(bool);
		bool GetScopeEffect();
		void InitPlayerData(RE::PlayerCharacter*, RE::PlayerCamera*);
		void StartScope(bool flag);
		void SetFinishAimAnim(bool flag);
		void SetInterfaceTextRefresh(bool flag);
		void QueryRender(bool flag) { bQueryRender = flag; }
		bool GetRenderState() { return bQueryRender ; }
		void SetIsUpscaler(bool flag) { bIsUpscaler = flag; }

		
	private:
		static OldFuncs oldFuncs;

		static RECT oldRect;
		static bool isShow;  
		static bool isEnableRender;
		static bool bStartScope;
		static bool bFinishAimAnim;
		static bool bRefreshChar;
		static int bEnableNVG;
		static bool bCanEnableNVGEffect;
		static bool bQueryRender;
		static bool bIsUpscaler;

	private:
		ComPtr<ID3D11InputLayout> m_pVertexLayout;
		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;

		ComPtr<ID3D11Buffer> m_pConstantBufferData = nullptr;
		ComPtr<ID3D11Buffer> m_pScopeEffectBuffer = nullptr;
		ComPtr<ID3D11Buffer> m_pGameConstBuffer = nullptr;
		ComPtr<ID3D11Buffer> m_VSBuffer = nullptr;
		ComPtr<ID3D11Buffer> m_VSOutBuffer = nullptr;

		ComPtr<ID3D11VertexShader> m_pVertexShader;
		ComPtr<ID3D11PixelShader> m_pPixelShader;
		ComPtr<ID3D11PixelShader> m_outPutPixelShader;
		ComPtr<ID3D11VertexShader> m_outPutVertexShader;
		ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer; 
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		ComPtr<ID3D11SamplerState> m_pSamplerState; 
		ComPtr<ID3D11SamplerState> m_pSamplerStateReticle; 

		ComPtr<ID3D11BlendState> BSAlphaToCoverage; 
		ComPtr<ID3D11BlendState> BSTransparent; 

		ComPtr<ID3D11Texture2D> mTextDDS;
		ComPtr<ID3D11Resource> mTextDDS_Res;

		ID3D11Texture2D* mBackBuffer;
		ID3D11Texture2D* mBackBufferCopy;
		ID3D11Texture2D* mRealBackBuffer;


		ComPtr<ID3D11Texture2D> mDynamicTexture;
		D3D11_TEXTURE2D_DESC bbDesc;

		ComPtr<ID3D11ShaderResourceView> mTextDDS_SRV;
		ComPtr<ID3D11ShaderResourceView> mShaderResourceView;


		ScopeEffectShaderData scopeData;
		VSOutConstantData VSOutData;
		GameConstBuffer gameConstBuffer;
		ConstBufferData constBufferData;
		VSConstantData vsConstanData;
		
		ID3D11RenderTargetView* backBufferRTV;

		ComPtr<ID3D11Texture2D> mRTRenderTargetTexture;
		ComPtr<ID3D11RenderTargetView> mRTRenderTargetView;
		ComPtr<ID3D11ShaderResourceView> mRTShaderResourceView;

		ComPtr<ID3D11Texture2D> mOutPutRTRenderTargetTexture;
		ComPtr<ID3D11RenderTargetView> mOutPutRTRenderTargetView;
		ComPtr<ID3D11ShaderResourceView> mOutPutRTShaderResourceView;

	};
}
