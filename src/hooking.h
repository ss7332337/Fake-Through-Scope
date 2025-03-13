#pragma once

#include <stdint.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <ImGuiImpl.h>
#include "FTSData.h"
#include <REX/W32/COMPTR.hpp>


#define D3D11_HOOK_API
#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }
#ifndef HR
#	define HR(x) { HRESULT hr = (x);  if (FAILED(hr)) {_MESSAGE("[-] %s, %i, %i",__FILE__, __LINE__ , hr);}}
#endif

void SafeWriteBuf(uintptr_t addr, void* data, size_t len);
void SafeWrite64(uintptr_t addr, UInt64 data);
//void* GetIATAddr(void* module, const char* searchDllName, const char* searchImportName);

//template <class T>
//using ComPtr = Microsoft::WRL::ComPtr<T>;

template <class T>
using ComPtr = REX::W32::ComPtr<T>;

extern ComPtr<IDXGISwapChain> g_Swapchain;
extern ComPtr<ID3D11Device> g_Device;
extern ComPtr<ID3D11DeviceContext> g_Context;

namespace Hook
{
	
	HRESULT CreateShaderFromFile(
		const WCHAR* csoFileNameInOut,
		const WCHAR* hlslFileName,
		LPCSTR entryPoint,
		LPCSTR shaderModel,
		ID3DBlob** ppBlobOut);

	using namespace DirectX;

	class D3D
	{

		HRESULT WINAPI HookedD3D11CreateDeviceAndSwapChain(
			IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			const D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
			IDXGISwapChain** ppSwapChain,
			ID3D11Device** ppDevice,
			D3D_FEATURE_LEVEL* pFeatureLevel,
			ID3D11DeviceContext** ppImmediateContext);

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
		typedef void(__stdcall* tdefDrawIndexed)(ID3D11DeviceContext*, UINT, UINT, INT);
		typedef HRESULT(__stdcall* tdefCreateBuffer)(ID3D11Device*, struct D3D11_BUFFER_DESC const*, struct D3D11_SUBRESOURCE_DATA const*, struct ID3D11Buffer**);

		typedef HRESULT(__stdcall* D3D11PresentHook)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
		typedef void(__stdcall* D3D11DrawIndexedHook)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		typedef void(__stdcall* D3D11CreateQueryHook)(ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery);
		typedef void(__stdcall* D3D11PSSetShaderResourcesHook)(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews);
		typedef void(__stdcall* D3D11ClearRenderTargetViewHook)(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]);
		typedef HRESULT(WINAPI* CreateVertexShader_t)(ID3D11Device* device, const void* pShaderBytecode, SIZE_T bytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader);
		typedef HRESULT(WINAPI* CreatePixelShader_t)(ID3D11Device* device, const void* pShaderBytecode, SIZE_T bytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader);



	public:
		tdefDrawIndexed tdefDrawIndexed_t;
		

	private:
		static std::once_flag flagOnce;

	public:
		
		static D3D* GetSington();

		static [[nodiscard]] bool Register() noexcept;

	
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

	public:

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
			float baseFovAdjustTarget = 0;

			XMFLOAT2 ScopeEffect_Size = { 0, 0 };
			XMFLOAT2 ScopeEffect_OriPositionOffset = { 0, 0 };
			XMFLOAT2 ScopeEffect_OriSize = { 0, 0 };
			XMFLOAT2 ScopeEffect_Offset = { 0, 0 };

			XMFLOAT3 eyeDirection = {0,0,0};
			float targetAdjustFov = 0;

			XMFLOAT3 eyeDirectionLerp = {0,0,0};
			float padding2 = 0;
			XMFLOAT3 eyeTranslationLerp = { 0, 0, 0 };
			float padding3 = 0;
			XMFLOAT3 CurrWeaponPos = { 0, 0, 0 };
			float padding4 = 0;
			XMFLOAT3 CurrRootPos = { 0, 0, 0 };
			float padding5 = 0;
			XMFLOAT4X4 CameraRotation = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

			XMFLOAT2 FTS_ScreenPos = { 0, 0 };
			XMFLOAT2 reticle_Offset = { 0, 0 };

			XMFLOAT4X4 projMat;	
			XMFLOAT4 rect;
		};


		struct GameConstBuffer
		{
			RE::NiPoint3 virDir;
			RE::NiPoint3 lastVirDir;
			RE::NiPoint3 VirDirLerp;
			RE::NiPoint3 VirTransLerp;
			RE::NiPoint3 weaponPos;
			RE::NiPoint3 rootPos;
			RE::NiPoint3 ftsScreenPos;
			RE::NiMatrix3 camMat;
			RE::NiMatrix3 ftsLocalMat;
			RE::NiMatrix3 ftsWorldMat;
			float deltaZoom;
		};

	public:

		D3D11_HOOK_API void ImplHookDX11_Init(HMODULE hModule, void* hwnd);
		//void __stdcall Hook();
		void EnableRender(bool flag) { isEnableRender = flag; }
		void Render();
		bool InitEffect();
		void InitLegacyEffect();
		bool InitResource();
		void InitInputLayout();
		void OnResize();
		void UpdateScene(ScopeData::FTSData*);
		void ShowMenu(bool);
		void EnableCursor(bool enable);
		static void RenderImGui();
		bool GetIsShow() { return isShow; };
		void CreateBlender();
		void QueryChangeReticleTexture();
		void ResetZoomDelta();
		void GrabScreen();
		void RenderToReticleTexture();
		void RenderToReticleTextureNew(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		void MapScopeEffectBuffer(ScopeEffectShaderData);
		void detourDirectXPresent();
		void detourDirectXDrawIndexed();
		void CreateDx12Device();

		RE::NiPoint3 WorldToScreen(RE::NiAVObject* cam, RE::NiAVObject* obj, float fov);

	private:
		D3D() {}
		~D3D() {}

		D3D(const D3D&) = delete;
		D3D(D3D&&) = delete;

		D3D& operator=(const D3D&) = delete;
		D3D& operator=(D3D&&) = delete;

		static HRESULT __stdcall D3D11CreateDeviceAndSwapChainHook(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
		static LRESULT __stdcall WndProcHandler(HWND, UINT, WPARAM, LPARAM);
		static BOOL __stdcall ClipCursorHook(RECT*);
		static void __stdcall vsSetConstantBuffers(ID3D11DeviceContext* pContext, UINT, UINT, ID3D11Buffer* const*);

		static HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
		static void __stdcall DrawIndexedHook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		static void __stdcall hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery);
		static void __stdcall hookD3D11PSSetShaderResources(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews);
		static void __stdcall ClearRenderTargetViewHook(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]);
		static HRESULT WINAPI CreateVertexShaderHook(ID3D11Device* device, const void* pShaderBytecode, SIZE_T bytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader);
		static HRESULT WINAPI CreatePixelShaderHook(ID3D11Device* device, const void* pShaderBytecode, SIZE_T bytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader);

		DWORD __stdcall HookDX11_Init();
		void InitializeImGui(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);

		struct VSConstantBufferSlot
		{
			UINT StartSlot;
			UINT NumBuffers;
			ID3D11Buffer* const* ppBuffers;
		};

		struct Vertex
		{
			float x, y, z;  // 位置
			float r, g, b;  // 颜色
		};

		// 顶点数据
		const Vertex gdc_Vertices[4] = {
			{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f },  // 左下角
			{ -1.0f, 3.0f, 0.0f, 0.0f, 2.0f },   // 左上角
			{ 3.0f, -1.0f, 0.0f, 2.0f, 0.0f }    // 右下角
		};

		void SetupCommonRenderState(
			ID3D11Resource* sourceTexture, ID3D11VertexShader* vs, ID3D11ClassInstance* const* vsClassInstances, UINT vsClassInstancesCount,
			ID3D11PixelShader* ps, ID3D11InputLayout* inputLayout, ID3D11BlendState* blendState, const std::vector<VSConstantBufferSlot>& vsCBSlots,
			ID3D11Buffer* indexBuffer,
			DXGI_FORMAT indexFormat,
			UINT indexOffset,
			ID3D11Buffer* const* vertexBuffers,
			const UINT* strides,
			const UINT* offsets,
			UINT numVertexBuffers);

		void LoadAimTexture(const std::string& path);
		template <typename T>
		void UpdateConstantBuffer(const ComPtr<ID3D11Buffer>& buffer, const T& data);
		void UpdateGameConstants(const GameConstBuffer& src, ScopeEffectShaderData& dst);
		void UpdateVsConstants(const GameConstBuffer& src, VSConstantData& dst);

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
			tdefCreateBuffer createBuffer;

			D3D11PresentHook phookD3D11Present = nullptr;
			D3D11DrawIndexedHook phookD3D11DrawIndexed = nullptr;
			D3D11CreateQueryHook phookD3D11CreateQuery = nullptr;
			D3D11PSSetShaderResourcesHook phookD3D11PSSetShaderResources = nullptr;
			D3D11ClearRenderTargetViewHook phookD3D11ClearRenderTargetViewHook = nullptr;
			CreateVertexShader_t phookD3D11VertexShaderHook = nullptr;
			CreatePixelShader_t phookD3D11PixelShaderHook = nullptr;
		};

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
		void SetIsInGame(bool flag) { bIsInGame = flag; }
		GameConstBuffer* GetGameConstBuffer() { return &gameConstBuffer; }
		void SetImGuiImplClass(ImGuiImpl::ImGuiImplClass* imguiImplClass);

	public:
		static bool bLegacyMode;
		static bool isEnableScopeEffect;
		static bool bEnableEditMode;
		static bool bRefreshChar;
		static bool isEnableRender;

	private:
		static OldFuncs oldFuncs;

		static RECT oldRect;
		static bool isShow;  
		
		static bool bStartScope;
		static bool bFinishAimAnim;
		static int bEnableNVG;
		static bool bCanEnableNVGEffect;
		static bool bQueryRender;
		static bool bIsUpscaler;
		static bool bIsInGame;

	private:
		ID3D11Texture2D* m_DstTexture = nullptr;
		ID3D11ShaderResourceView* m_DstView = nullptr;

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

		ComPtr<ID3D11PixelShader> m_pPixelShader_Legacy;
		ComPtr<ID3D11PixelShader> m_outPutPixelShader_Legacy;
		ComPtr<ID3D11VertexShader> m_pVertexShader_Legacy;

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
