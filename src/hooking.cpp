#include "hooking.h"
#include "DDSTextureLoader11.h"
#include "WICTextureLoader11.h"

#include "MathUtils.h"
#include <vector>
#include <Shlwapi.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "imgui.h"
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <misc/cpp/imgui_stdlib.h>

//#include <dinput.h>

#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dwrite.lib")
//#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


inline void InitCurrentScopeData(RE::BGSKeyword* animFlavorKeyword);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool UsingSTS_UI;
int scopeFrame_UI;
bool IsCircle_UI;
float camDepth_UI;
float ReticleSize_UI;
float minZoom_UI;
float maxZoom_UI;
float PositionOffset_UI[2];
float OriPositionOffset_UI[2];
float Size_UI[2];
float OriSize_UI[2];
float radius_UI;
float relativeFogRadius_UI;
float scopeSwayAmount_UI;
float maxTravel_UI;

bool bEnableZMove;
bool isActive_TAA;
bool bEnableNVGEffect;
float nvIntensity_UI;
float baseWeaponPos_UI;
float MovePercentage_UI;

const char* const mainKey[] = {
	"None",
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
};

static const char* current_item = NULL;
static const char* current_itemA = NULL;

bool isInitNVGComboKey = false;
bool isInitNVGMainKey = false;
bool bReloadRenderMode = false;
bool bDisableWhileBolt = false;
bool bChangeAimTexture = true;
bool bResetZoomDelta = false;
bool bSelfDraw = false;

int nvgComboKeyIndex = 0;
int nvgMainKeyIndex = 0;
int guiKeyIndex = 0;

//vertex
ID3D11Buffer* veBuffer;
UINT Stride;
UINT veBufferOffset;
D3D11_BUFFER_DESC vedesc;

//index
ID3D11Buffer* inBuffer;
DXGI_FORMAT inFormat;
UINT inOffset;
D3D11_BUFFER_DESC indesc;

//psgetConstantbuffers
UINT pscStartSlot;
ID3D11Buffer* pscBuffer;
D3D11_BUFFER_DESC pscdesc;

//vsgetconstantbuffers
UINT vscStartSlot;
ID3D11Buffer* vscBuffer;
D3D11_BUFFER_DESC vscdesc;
D3D11_BUFFER_DESC vscdesc1;
D3D11_BUFFER_DESC vscdesc2;

ID3D11VertexShader* draw_vsShader;


using namespace DirectX;

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<IDXGISwapChain> g_Swapchain = nullptr;
ComPtr < ID3D11Device> g_Device = nullptr;
ComPtr<ID3D11DeviceContext> g_Context = nullptr;


UINT m_frameCount = 0;
UINT m_BackBuffCount = 0;
int RenderCount = 0;

std::vector<ID3D11RenderTargetView*> g_ppRenderTargetView(8);
bool depthStencilViewUpdate = true;

int windowWidth;
int windowHeight;

XMVECTORF32 bgColor = { 0.1f, 0.1f, 0.1f, 0.0f };
long unsigned int gCaptureNum = 0;

bool isEnableScopeEffect = false;
bool bEnableEditMode = false;

float dtimer = 0;
float gameZoomDelta = 1;
bool bIsFirst = true;

//logger, misc
//bool logger = false;
int countnum = -1;
int countStride = -1;
int countIndexCount = -1;
int countpscdescByteWidth = -1;
int countindescByteWidth = -1;
int countvedescByteWidth = -1;


//RTVertex quadVertices[4] = {
//	{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
//	{ XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
//	{ XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
//	{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f) }
//};

struct RTVertex
{
	XMFLOAT3 position;  // 位置
	XMFLOAT2 texcoord;  // 纹理坐标
};


XMFLOAT4X4 localTestingMat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

//RTVertex vertices[] = {
//	{ XMFLOAT3(-0.5f, -0.5f, 1.0f), XMFLOAT2(0.0f, 1.0f) },  // 左下
//	{ XMFLOAT3(0.5f, -0.5f, 1.0f) , XMFLOAT2(1.0f, 1.0f) },   // 左上
//	{ XMFLOAT3(-0.5f, 0.5f, 1.0f) , XMFLOAT2(0.0f, 0.0f) },   // 右下
//	{ XMFLOAT3(-0.5f, 0.5f, 1.0f) , XMFLOAT2(0.0f, 0.0f) },   // 右下
//	{ XMFLOAT3(0.5f, -0.5f, 1.0f) , XMFLOAT2(1.0f, 1.0f) },   // 左上
//	{ XMFLOAT3(0.5f, 0.5f, 1.0f)  , XMFLOAT2(1.0f, 0.0f) }     // 右上
//};

D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(RTVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(RTVertex, texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

using namespace ScopeData;

#pragma region writeThunk

void SafeWriteBuf(uintptr_t addr, void* data, size_t len)
{
	UInt32 oldProtect;
	
	VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((void*)addr, data, len);
	VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
}
	
void SafeWrite64(uintptr_t addr, UInt64 data)
{
	SafeWriteBuf(addr, &data, sizeof(data));
}
	
void* GetIATAddr(void* module, const char* searchDllName, const char* searchImportName)
{
	UInt8* base = (UInt8*)module;
	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)base;
	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
	IMAGE_IMPORT_DESCRIPTOR* importTable =
		(IMAGE_IMPORT_DESCRIPTOR*)(base + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	
	for (; importTable->Characteristics; ++importTable) {
		const char* dllName = (const char*)(base + importTable->Name);
	
		if (!_stricmp(dllName, searchDllName)) {
			// found the dll
	
			IMAGE_THUNK_DATA* thunkData = (IMAGE_THUNK_DATA*)(base + importTable->OriginalFirstThunk);
			uintptr_t* iat = (uintptr_t*)(base + importTable->FirstThunk);
	
			for (; thunkData->u1.Ordinal; ++thunkData, ++iat) {
				if (!IMAGE_SNAP_BY_ORDINAL(thunkData->u1.Ordinal)) {
					IMAGE_IMPORT_BY_NAME* importInfo = (IMAGE_IMPORT_BY_NAME*)(base + thunkData->u1.AddressOfData);
	
					if (!_stricmp((char*)importInfo->Name, searchImportName)) {
						// found the import
						return iat;
					}
				}
			}
	
			return NULL;
		}
	}
	
	return NULL;
}
	
inline std::uintptr_t HookFunc(std::uintptr_t* vtbl, int index, std::uintptr_t hookFn, std::uintptr_t* oldFn)
{
	std::uintptr_t returnAddy = vtbl[index];
	*oldFn = returnAddy;
	
	SafeWrite64((std::uintptr_t)(vtbl + index), hookFn);
	
	return returnAddy;
}
	
	
template <class T>
void write_thunk_call(std::uintptr_t a_src)
{
	F4SE::AllocTrampoline(14);
	
	auto& trampoline = F4SE::GetTrampoline();
	T::func = trampoline.write_call<5>(a_src, T::thunk);
}
	
template <class T>
void write_thunk_call6(std::uintptr_t a_src)
{
	F4SE::AllocTrampoline(14);
	
	auto& trampoline = F4SE::GetTrampoline();
	T::func = trampoline.write_call<6>(a_src, T::thunk);
}
	
template <class F, std::size_t idx, class T>
void write_vfunc()
{
	REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
	T::func = vtbl.write_vfunc(idx, T::thunk);
}
	
template <std::uintptr_t address, std::size_t idx, class T>
void write_vfunc()
{
	REL::Relocation<std::uintptr_t> vtbl{ address };
	T::func = vtbl.write_vfunc(idx, T::thunk);
}
	
template <std::size_t idx, class T>
void write_vfunc(std::uintptr_t id)
{
	REL::Relocation<std::uintptr_t> vtbl{ id };
	T::func = vtbl.write_vfunc(idx, T::thunk);
}
#pragma endregion

char tempbuf[8192] = { 0 };
char* _MESSAGE(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsnprintf(tempbuf, sizeof(tempbuf), fmt, args);
	va_end(args);
	spdlog::log(spdlog::level::warn, tempbuf);

	return tempbuf;
}

namespace Hook
{
	RE::PlayerCharacter* player;
	RE::PlayerCamera* pcam;
	ScopeDataHandler* sdh;

	

#ifndef HR
#	define HR(x) { HRESULT hr = (x);  if (FAILED(hr)) {_MESSAGE("%s, %i, %i",__FILE__, __LINE__ , hr);}}
#endif

	const wchar_t* GetWC(const char* c)
	{
		size_t len = strlen(c) + 1;

		size_t converted = 0;

		wchar_t* WStr;

		WStr = (wchar_t*)malloc(len * sizeof(wchar_t));

		mbstowcs_s(&converted, WStr, len, c, _TRUNCATE);
		return WStr;
	}

	HRESULT CreateShaderFromFile(const WCHAR* csoFileNameInOut,const WCHAR* hlslFileName,LPCSTR entryPoint,LPCSTR shaderModel,ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK) {
			return hr;
		} else {
			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG

			dwShaderFlags |= D3DCOMPILE_DEBUG;

			dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			ID3DBlob* errorBlob = nullptr;
			hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
				dwShaderFlags, 0, ppBlobOut, &errorBlob);
			if (FAILED(hr)) {
				if (errorBlob != nullptr) {
					OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
				}
				SAFE_RELEASE(errorBlob);
				return hr;
			}

			if (csoFileNameInOut) {
				return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
			}
		}

		return hr;
	}

	void D3D::EnableCursor(bool enable)
	{
		static bool e{};

		if (e != enable)
			::ShowCursor(enable);

		e = enable;
	}
	LRESULT __stdcall D3D::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg) {
			case WM_KEYDOWN:
				{
					bool isPressed = (lParam & 0x40000000) != 0x0;
					if (!isPressed && wParam == sdh->guiKey) {
						isShow = !isShow;

						//StayInADS();
						instance().EnableCursor(isShow);

						auto input = (RE::BSInputDeviceManager::GetSingleton());
						if (input) {
							input->valueQueued = !isShow;
							input->pollingEnabled = !isShow;					
						}
					}
				}
				break;
			case WM_MOUSEWHEEL:
				float zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
				if (zDelta > 0)
					gameZoomDelta += 0.1F;
				else if (zDelta< 0)
					gameZoomDelta -= 0.1F;
				break;
		}

		ImGuiIO& io = ImGui::GetIO();

		if (isShow && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return TRUE;
	
		return CallWindowProc(oldFuncs.wndProc,hWnd, msg, wParam, lParam);
	}
	bool D3D::InitEffect()
	{
		ComPtr<ID3DBlob> blob;

		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_PS.cso", L"HLSL\\ScopeEffect_PS.hlsl", "main", "ps_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_PS_Output.cso", L"HLSL\\ScopeEffect_PS_Output.hlsl", "main", "ps_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_outPutPixelShader.GetAddressOf()));

		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_VS.cso", L"HLSL\\ScopeEffect_VS.hlsl", "main", "vs_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
		
		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_VS_Output.cso", L"HLSL\\ScopeEffect_VS_Output.hlsl", "main", "vs_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_outPutVertexShader.GetAddressOf()));

		HR(g_Device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements),
			blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

		return true;
	}
	void D3D::CreateBlender()
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		auto& rtDesc = blendDesc.RenderTarget[0];

		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HR(g_Device->CreateBlendState(&blendDesc, BSAlphaToCoverage.GetAddressOf()));

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		rtDesc.BlendEnable = true;
		rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

		HR(g_Device->CreateBlendState(&blendDesc, BSTransparent.GetAddressOf()));
	}

	bool D3D::InitResource()
	{

		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(cbd));
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(ScopeEffectShaderData);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(g_Device->CreateBuffer(&cbd, nullptr, m_pScopeEffectBuffer.GetAddressOf()));

		D3D11_BUFFER_DESC cbd1;
		ZeroMemory(&cbd1, sizeof(cbd1));
		cbd1.Usage = D3D11_USAGE_DYNAMIC;
		cbd1.ByteWidth = sizeof(ConstBufferData);
		cbd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(g_Device->CreateBuffer(&cbd1, nullptr, m_pConstantBufferData.GetAddressOf()));

		D3D11_BUFFER_DESC cbd2;
		ZeroMemory(&cbd2, sizeof(cbd2));
		cbd2.Usage = D3D11_USAGE_DYNAMIC;
		cbd2.ByteWidth = sizeof(VSConstantData);
		cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(g_Device->CreateBuffer(&cbd2, nullptr, m_VSBuffer.GetAddressOf()));

		D3D11_BUFFER_DESC cbd3;
		ZeroMemory(&cbd3, sizeof(cbd3));
		cbd3.Usage = D3D11_USAGE_DYNAMIC;
		cbd3.ByteWidth = sizeof(VSOutConstantData);
		cbd3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd3.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(g_Device->CreateBuffer(&cbd3, nullptr, m_VSOutBuffer.GetAddressOf()));


		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(g_Device->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf()));


		CreateBlender();

		

		return true;
	}

	void D3D::InitInputLayout()
	{

	}

	void D3D::OnResize()
	{

		m_pRenderTargetView.Reset();
		m_pDepthStencilView.Reset();
		m_pDepthStencilBuffer.Reset();

		//g_Swapchain->ResizeBuffers(1, RE::BSGraphics::RendererData::GetSingleton()->renderWindow->windowWidth, RE::BSGraphics::RendererData::GetSingleton()->renderWindow->windowWidth, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		/*g_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(mBackBuffer.GetAddressOf()));
		HR(g_Device->CreateRenderTargetView(mBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

		mBackBuffer.Reset();*/

		D3D11_TEXTURE2D_DESC depthStencilDesc;

		depthStencilDesc.Width = windowWidth;
		depthStencilDesc.Height = windowHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;

		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HR(g_Device->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf()));
		HR(g_Device->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf()));



		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = windowWidth;
		textureDesc.Height = windowHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HR(g_Device->CreateTexture2D(&textureDesc, NULL, mRTRenderTargetTexture.GetAddressOf()));

		//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		HR(g_Device->CreateRenderTargetView(mRTRenderTargetTexture.Get(), &renderTargetViewDesc, mRTRenderTargetView.GetAddressOf()));

		//第三,填充着色器资源视图形容体,并进行创建着色器资源视图
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		HR(g_Device->CreateShaderResourceView(mRTRenderTargetTexture.Get(), &shaderResourceViewDesc, mRTShaderResourceView.GetAddressOf()));


		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = 1024;
		textureDesc.Height = 1024;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HR(g_Device->CreateTexture2D(&textureDesc, NULL, mOutPutRTRenderTargetTexture.GetAddressOf()));
		HR(g_Device->CreateRenderTargetView(mOutPutRTRenderTargetTexture.Get(), &renderTargetViewDesc, mOutPutRTRenderTargetView.GetAddressOf()));
		HR(g_Device->CreateShaderResourceView(mOutPutRTRenderTargetTexture.Get(), &shaderResourceViewDesc, mOutPutRTShaderResourceView.GetAddressOf()));

		
	}

	ID3D11RenderTargetView* tempRt;
	ID3D11DepthStencilView* tempSV;

	ID3D11RenderTargetView* tempRt1;
	ID3D11DepthStencilView* tempSV1;

	bool bHasGrab = false;

	int myDrawCount = 0;
	UINT targetIndexCount = 0;
	UINT targetStartIndexLocation = 0;
	UINT targetBaseVertexLocation = 0;
	ComPtr<ID3D11VertexShader> targetVS;
	ComPtr<ID3D11ClassInstance> targetVSClassInstance;
	UINT targetVSNumClassesInstance;

	ComPtr<ID3D11PixelShader> targetPS;
	ComPtr<ID3D11DepthStencilState> targetDepthStencilState = nullptr;
	ComPtr<ID3D11InputLayout> targetInputLayout;

	ComPtr<ID3D11Buffer> targetIndexBuffer;
	DXGI_FORMAT targetIndexBufferFormat;
	UINT targetIndexBufferOffset;

	ComPtr<ID3D11Buffer> targetVertexBuffer;
	UINT targetVertexBufferStrides;
	UINT targetVertexBufferOffsets;

	ComPtr <ID3D11Buffer> targetVertexConstBuffer;
	ComPtr <ID3D11Buffer> targetVertexConstBuffer1p5;
	ComPtr <ID3D11Buffer> targetVertexConstBuffer1;

	ComPtr<ID3D11Buffer> targetVertexConstBufferOutPut;
	ComPtr<ID3D11Buffer> targetVertexConstBufferOutPut1p5;
	ComPtr<ID3D11Buffer> targetVertexConstBufferOutPut1;
	
	ComPtr<ID3D11ShaderResourceView> DrawIndexedSRV;

	

	bool bHasDraw = false;
	ComPtr<ID3D11ShaderResourceView> targetSRV;


	void D3D::RenderToReticleTextureNew(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		if (!bIsFirst && isEnableRender && targetVS.Get())
		{
			
			HR(g_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(mBackBuffer.GetAddressOf())));
			//HR(g_Device->CreateRenderTargetView(mBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

			//g_Context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;  
			rtvDesc.Texture2D.MipSlice = 0;                         

			UINT srcWidth = windowWidth;
			UINT srcHeight = windowHeight;
			float outPutWidth = 2048;
			float outPutHeight = 2048;
			DXGI_FORMAT srcFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

			ID3D11Texture2D* pDstTexture = nullptr;
			ID3D11ShaderResourceView* pDstView = nullptr;
			D3D11_TEXTURE2D_DESC texDesc = {};
			texDesc.Width = outPutWidth;
			texDesc.Height = outPutHeight;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = srcFormat;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			HR(g_Device->CreateTexture2D(&texDesc, nullptr, &pDstTexture));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;

			HR(g_Device->CreateShaderResourceView(pDstTexture, &srvDesc, &pDstView));

			// 使用 CopySubresourceRegion 方法将图像资源的内容复制到新的 2D 纹理的指定区域
			UINT dstSubresource = D3D11CalcSubresource(0, 0, 1);  
			UINT srcSubresource = D3D11CalcSubresource(0, 0, 1);  
			UINT dstZ = 0;                                        

			// 计算源纹理和目标纹理的宽高比
			float srcAspect = (float)srcWidth / (float)srcHeight;            // 源纹理的宽高比
			float dstAspect = (float)texDesc.Width / (float)texDesc.Height;  // 目标纹理的宽高比

			// 根据比例调整复制的区域大小
			UINT copyWidth, copyHeight;  // 复制区域的宽高
			if (srcAspect > dstAspect) {
					// 源纹理比目标纹理更宽，所以要裁剪左右两边
					copyWidth = (UINT)(dstAspect * (float)srcHeight);
					copyHeight = srcHeight;
			} else if (srcAspect < dstAspect) {
					// 源纹理比目标纹理更高，所以要裁剪上下两边
					copyWidth = srcWidth;
					copyHeight = (UINT)((float)srcWidth / dstAspect);
			} else {
					// 源纹理和目标纹理的宽高比相同，不需要裁剪
					copyWidth = srcWidth;
					copyHeight = srcHeight;
			}

			// 使用 D3D11_BOX 结构体来指定源区域的边界框
			D3D11_BOX srcBox = {};
			// 计算源纹理的中心坐标
			UINT srcCenterX = srcWidth / 2;
			UINT srcCenterY = srcHeight / 2;

			// 使用源纹理的中心坐标来偏移源区域的左右上下边界
			srcBox.left = srcCenterX - copyWidth / 2;     // 源区域左边界
			srcBox.right = srcCenterX + copyWidth / 2;    // 源区域右边界
			srcBox.top = srcCenterY - copyHeight / 2;     // 源区域上边界
			srcBox.bottom = srcCenterY + copyHeight / 2;  // 源区域下边界
			srcBox.front = 0;                             // must be zero
			srcBox.back = 1;                              // muse be one

			// 计算目标纹理的中心坐标
			UINT dstCenterX = texDesc.Width / 2;
			UINT dstCenterY = texDesc.Height / 2;

			copyWidth = min(copyWidth, texDesc.Width);
			copyHeight = min(copyHeight, texDesc.Height);

			// 计算目标区域左上角的坐标
			UINT dstX = dstCenterX - copyWidth / 2;
			UINT dstY = dstCenterY - copyHeight / 2;

			// 调整目标区域的偏移量，使其不小于零
			dstX = max(dstX, 0);
			dstY = max(dstY, 0);

			// 使用这个坐标作为 CopySubresourceRegion 方法的 dstX 和 dstY 参数
			g_Context->CopySubresourceRegion(pDstTexture, dstSubresource, dstX, dstY, dstZ, mRTRenderTargetTexture.Get(), srcSubresource, &srcBox);

			// 如果需要，可以使用 GenerateMips 方法为新的 2D 纹理生成 mipmap 链
			//g_Context->GenerateMips(pDstView);

			// 现在你可以使用 pDstTexture 和 pDstView 来渲染或其他操作了


			//mBackBuffer.ReleaseAndGetAddressOf();

			g_Context->VSSetShader(targetVS.Get(), targetVSClassInstance.GetAddressOf(), targetVSNumClassesInstance);
			g_Context->PSSetShader(m_outPutPixelShader.Get(), nullptr, 0);

			D3D11_DEPTH_STENCIL_DESC tempDSD;
			D3D11_DEPTH_STENCILOP_DESC tempDSOPD;
			tempDSOPD.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			tempDSOPD.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			tempDSOPD.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			tempDSOPD.StencilFunc = D3D11_COMPARISON_ALWAYS;

			tempDSD.DepthEnable = false;
			tempDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			tempDSD.DepthFunc = D3D11_COMPARISON_LESS;
			tempDSD.StencilEnable = false;
			tempDSD.StencilReadMask = 255;
			tempDSD.StencilWriteMask = 255;
			tempDSD.FrontFace = tempDSOPD;
			tempDSD.BackFace = tempDSOPD;


			HR(g_Device->CreateDepthStencilState(&tempDSD, targetDepthStencilState.GetAddressOf()));

			g_Context->OMSetDepthStencilState(targetDepthStencilState.Get(), 0);

			g_Context->IASetInputLayout(targetInputLayout.Get());
			g_Context->VSSetConstantBuffers(1, 1, targetVertexConstBufferOutPut.GetAddressOf());
			g_Context->VSSetConstantBuffers(2, 1, targetVertexConstBufferOutPut1p5.GetAddressOf());
			g_Context->VSSetConstantBuffers(12, 1, targetVertexConstBufferOutPut1.GetAddressOf());
			g_Context->IASetIndexBuffer(targetIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, targetIndexBufferOffset);
			g_Context->IASetVertexBuffers(0, 1, targetVertexBuffer.GetAddressOf(), &targetVertexBufferStrides, &targetVertexBufferOffsets);

			g_Context->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
			g_Context->PSSetShaderResources(0, 1, &pDstView);
			g_Context->PSSetShaderResources(5, 1, &pDstView);

			/*bSelfDraw = true;
			g_Context->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
			bSelfDraw = false;*/

			if (pDstTexture)
				pDstTexture->Release();
			if (pDstView)
				pDstView->Release();
			
			//g_Context->OMSetRenderTargets(1, &tempRt, tempSV);
			//m_pRenderTargetView.ReleaseAndGetAddressOf();
		}
			
	}

	void D3D::UpdateScene()
	{
		auto currData = sdh->GetCurrentFTSData();
		 
		if (!currData)
			return;

		g_Context->Flush();
		g_Context->OMGetRenderTargets(1, &tempRt, &tempSV);

		ID3D11Resource* currRTResource;
		tempRt->GetResource(&currRTResource);
		//ID3D11Texture2D* currRTtexture = mBackBuffer.Get();
		ID3D11Texture2D* currRTtexture = (ID3D11Texture2D*)currRTResource;

		g_Context->OMSetRenderTargets(1, mRTRenderTargetView.GetAddressOf(), tempSV);


		/*FLOAT clearColor[4] = { 0, 0, 0, 0 };
		g_Context->ClearRenderTargetView(mRTRenderTargetView.Get(), clearColor);*/
		/*if (isActive_TAA)
			g_Context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);*/

		HR(g_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(mBackBuffer.GetAddressOf())));
		mBackBuffer->GetDesc(&bbDesc);
		bbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bbDesc.CPUAccessFlags = 0;
		bbDesc.Usage = D3D11_USAGE_DEFAULT;
		bbDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

		HR(g_Device->CreateTexture2D(&bbDesc, nullptr, &mDynamicTexture));
		////g_Context->CopyResource(mRTRenderTargetTexture.Get(), mBackBuffer.Get());
		g_Context->CopyResource(mDynamicTexture.Get(), currRTtexture);
		HR(g_Device->CreateShaderResourceView(currRTtexture, NULL, mShaderResourceView.GetAddressOf()));
		
		g_Context->OMSetBlendState(BSTransparent.Get(), nullptr, 0xFFFFFFFF);

		
		g_Context->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
		g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_Context->PSSetConstantBuffers(0, 1, m_pScopeEffectBuffer.GetAddressOf());
		g_Context->VSSetConstantBuffers(1, 1, m_VSBuffer.GetAddressOf());
		g_Context->PSSetConstantBuffers(2, 1, m_pConstantBufferData.GetAddressOf());

		g_Context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		g_Context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
		g_Context->IASetInputLayout(m_pVertexLayout.Get());

#pragma region FO4GameConstantBuffer


		if (isEnableScopeEffect) 
		{
			scopeData.EnableMerge = 1;
		} 
		else if (!isEnableScopeEffect) 
		{
			scopeData.ScopeEffect_Zoom = currData->shaderData.minZoom;
			scopeData.EnableMerge = 0;
		}

		float tempCamMat[4][4] = {
			{ gameConstBuffer.camMat.entry[0].pt[0], gameConstBuffer.camMat.entry[0].pt[1], gameConstBuffer.camMat.entry[0].pt[2] ,0},
			{ gameConstBuffer.camMat.entry[1].pt[0], gameConstBuffer.camMat.entry[1].pt[1], gameConstBuffer.camMat.entry[1].pt[2] ,0},
			{ gameConstBuffer.camMat.entry[2].pt[0], gameConstBuffer.camMat.entry[2].pt[1], gameConstBuffer.camMat.entry[2].pt[2] ,0},
			{ 0, 0, 0 ,1}
		};


		memcpy_s(&scopeData.CameraRotation, sizeof(float) * 16, tempCamMat, sizeof(float) * 16);
		//scopeData.CameraRotation = tempCamMat;
		scopeData.CurrRootPos = gameConstBuffer.rootPos.GetXMFLOAT3();
		scopeData.CurrWeaponPos = gameConstBuffer.weaponPos.GetXMFLOAT3();
		scopeData.eyeDirection = gameConstBuffer.virDir.GetXMFLOAT3();
		scopeData.eyeDirectionLerp = { gameConstBuffer.VirDirLerp.x, gameConstBuffer.VirDirLerp.y, gameConstBuffer.VirDirLerp.z };
		scopeData.eyeTranslationLerp = gameConstBuffer.VirTransLerp.GetXMFLOAT3();

		if (bResetZoomDelta) {
			gameZoomDelta = currData->shaderData.minZoom;
			bResetZoomDelta = false;
		}

		if (gameZoomDelta >= currData->shaderData.maxZoom)
			gameZoomDelta = currData->shaderData.maxZoom;
		else if (gameZoomDelta <= currData->shaderData.minZoom)
			gameZoomDelta = currData->shaderData.minZoom;

		scopeData.ScopeEffect_Zoom = gameZoomDelta;
		scopeData.GameFov = pcam->firstPersonFOV;
#pragma endregion

		
#pragma region MyScopeShaderData
		if (!bEnableEditMode) {
			scopeData.BaseWeaponPos = currData->shaderData.baseWeaponPos;
			scopeData.camDepth = currData->shaderData.camDepth;
			scopeData.EnableNV = currData->shaderData.bCanEnableNV ? bEnableNVG : 0;
			scopeData.EnableZMove = currData->shaderData.bEnableZMove;
			scopeData.isCircle = currData->shaderData.IsCircle;
			scopeData.MovePercentage = currData->shaderData.movePercentage;
			scopeData.nvIntensity = currData->shaderData.nvIntensity;
			scopeData.parallax_maxTravel = currData->shaderData.parallax.maxTravel;
			scopeData.parallax_Radius = currData->shaderData.parallax.radius;
			scopeData.parallax_relativeFogRadius = currData->shaderData.parallax.relativeFogRadius;
			scopeData.parallax_scopeSwayAmount = currData->shaderData.parallax.scopeSwayAmount;
			scopeData.ReticleSize = currData->shaderData.ReticleSize;
			scopeData.ScopeEffect_Offset = { currData->shaderData.PositionOffset[0], currData->shaderData.PositionOffset[1] };
			scopeData.ScopeEffect_OriPositionOffset = { currData->shaderData.OriPositionOffset[0], currData->shaderData.OriPositionOffset[1] };
			scopeData.ScopeEffect_OriSize = { currData->shaderData.OriSize[0], currData->shaderData.OriSize[1] };
			scopeData.ScopeEffect_Size = { currData->shaderData.Size[0], currData->shaderData.Size[1] };

			vsConstanData.CurrRootPos = gameConstBuffer.rootPos.GetXMFLOAT3();
			vsConstanData.CurrWeaponPos = gameConstBuffer.weaponPos.GetXMFLOAT3();
			vsConstanData.eyeDirection = gameConstBuffer.virDir.GetXMFLOAT3();
			vsConstanData.eyeDirectionLerp = gameConstBuffer.VirDirLerp.GetXMFLOAT3();
			vsConstanData.MovePercentage1 = currData->shaderData.movePercentage;
			memcpy_s(&vsConstanData.CameraRotation, sizeof(float) * 16, tempCamMat, sizeof(float) * 16);

		}
#pragma endregion

		D3D11_MAPPED_SUBRESOURCE mappedDataA;
		HR(g_Context->Map(instance().m_pScopeEffectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataA));
		memcpy_s(mappedDataA.pData, sizeof(ScopeEffectShaderData), &instance().scopeData, sizeof(ScopeEffectShaderData));
		g_Context->Unmap(instance().m_pScopeEffectBuffer.Get(), 0);

		D3D11_MAPPED_SUBRESOURCE mappedDataB;
		HR(g_Context->Map(instance().m_VSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataB));
		memcpy_s(mappedDataB.pData, sizeof(VSConstantData), &instance().vsConstanData, sizeof(VSConstantData));
		g_Context->Unmap(instance().m_VSBuffer.Get(), 0);

		constBufferData.width = windowWidth;
		constBufferData.height = windowHeight;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(g_Context->Map(instance().m_pConstantBufferData.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(VSConstantData), &instance().constBufferData, sizeof(VSConstantData));
		g_Context->Unmap(instance().m_pConstantBufferData.Get(), 0);

		g_Context->PSSetShaderResources(4, 1, instance().mShaderResourceView.GetAddressOf());

		if (bChangeAimTexture) {
			if (instance().mTextDDS_SRV)
					instance().mTextDDS_SRV.ReleaseAndGetAddressOf();

			//g_Context->PSSetShaderResources(5, 1, nullptr);
			const wchar_t* tempPath;
			auto tempWchar = GetWC(currData->ZoomNodePath.c_str());
			tempPath = currData->ZoomNodePath.empty() ? L"Data/Textures/FTS/Empty.dds" : tempWchar;
			HR(CreateDDSTextureFromFile(g_Device.Get(), tempPath, nullptr, instance().mTextDDS_SRV.GetAddressOf()));
			
			if (tempWchar)
				free((void*)tempWchar);
			bChangeAimTexture = false;
		}
		g_Context->PSSetShaderResources(5, 1, instance().mTextDDS_SRV.GetAddressOf());

		mShaderResourceView.ReleaseAndGetAddressOf();
		mDynamicTexture.ReleaseAndGetAddressOf();
		mBackBuffer.ReleaseAndGetAddressOf();
		currRTtexture->Release();

		g_Context->Draw(3, 0);

		//instance().RenderToReticleTextureNew(targetIndexCount, targetStartIndexLocation, targetBaseVertexLocation);

		g_Context->OMSetRenderTargets(1, &tempRt, tempSV);

		/*HR(g_Device->CreateTexture2D(&bbDesc, nullptr, &mDynamicTexture));
		g_Context->CopyResource(mDynamicTexture.Get(), mRTRenderTargetTexture.Get());
		HR(g_Device->CreateShaderResourceView(mDynamicTexture.Get(), NULL, mShaderResourceView.GetAddressOf()));

		g_Context->PSSetShaderResources(0, 1, instance().mShaderResourceView.GetAddressOf());*/
		

	}

	void D3D::CreateRenderTarget()
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = 2048;
		textureDesc.Height = 2048;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HR(g_Device->CreateTexture2D(&textureDesc, NULL, instance().mRTRenderTargetTexture.GetAddressOf()));

		//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		HR(g_Device->CreateRenderTargetView(instance().mRTRenderTargetTexture.Get(), &renderTargetViewDesc, mRTRenderTargetView.GetAddressOf()));

		//第三,填充着色器资源视图形容体,并进行创建着色器资源视图
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		HR(g_Device->CreateShaderResourceView(instance().mRTRenderTargetTexture.Get(), &shaderResourceViewDesc, mShaderResourceView.GetAddressOf()));

	}

	void D3D::Render()
	{
		assert(g_Context);
		assert(g_Swapchain);

		if (bQueryRender) 
		{
			if (isEnableRender && pcam && player) 
			{
				if (bIsFirst) {
					OnResize();
					InitEffect();
					InitResource();
					bIsFirst = false;
				}

				UpdateScene();
				bHasDraw = true;
				//g_Context->Draw(3, 0);
				/*if (instance().mShaderResourceView)
					instance().mShaderResourceView.ReleaseAndGetAddressOf();*/
			}
			//bQueryRender = false;
			/*if (!isActive_TAA)
				g_Context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);*/
		}
		
	}

	void D3D::ResetUIData(FTSData* currData)
	{
		UsingSTS_UI = ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->UsingSTS;
		scopeFrame_UI = currData->scopeFrame;
		IsCircle_UI = currData->shaderData.IsCircle;
		camDepth_UI = currData->shaderData.camDepth;
		ReticleSize_UI = currData->shaderData.ReticleSize;
		minZoom_UI = currData->shaderData.minZoom;
		maxZoom_UI = currData->shaderData.maxZoom;
		PositionOffset_UI[0] = currData->shaderData.PositionOffset[0];
		PositionOffset_UI[1] = currData->shaderData.PositionOffset[1];
		OriPositionOffset_UI[0] = currData->shaderData.OriPositionOffset[0];
		OriPositionOffset_UI[1] = currData->shaderData.OriPositionOffset[1];
		Size_UI[0] = currData->shaderData.Size[0];
		Size_UI[1] = currData->shaderData.Size[1];
		OriSize_UI[0] = currData->shaderData.OriSize[0];
		OriSize_UI[1] = currData->shaderData.OriSize[1];

		radius_UI = currData->shaderData.parallax.radius;
		relativeFogRadius_UI = currData->shaderData.parallax.relativeFogRadius;
		scopeSwayAmount_UI = currData->shaderData.parallax.scopeSwayAmount;
		maxTravel_UI = currData->shaderData.parallax.maxTravel;

		bEnableZMove = currData->shaderData.bEnableZMove;
		_MESSAGE("bEnableNVGEffect: %i, currData->shaderData.bCanEnableNV: %i", bEnableNVGEffect, currData->shaderData.bCanEnableNV);
		bEnableNVGEffect = currData->shaderData.bCanEnableNV;
		nvIntensity_UI = currData->shaderData.nvIntensity;
		baseWeaponPos_UI = currData->shaderData.baseWeaponPos;
		MovePercentage_UI = currData->shaderData.movePercentage;
		bDisableWhileBolt = currData->shaderData.bBoltDisable;
		instance().bRefreshChar = false;
	}

	void D3D::RenderImGui()
	{
		if (isShow && isEnableRender) {
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Fake Throught Scope Adjusting Menu by XiFeiLi");
			auto currData = ScopeDataHandler::GetSingleton()->GetCurrentFTSData();
			if (!currData) {
				ImGui::TextUnformatted("This is not FTS Supported Weapon, Consider make a Patch or Double Check your Patch!");
				ImGui::NewLine();
				ImGui::End();
				ImGui::Render();
				return;
			}

			/*float textMatRow1[4] = { localTestingMat._11, localTestingMat._12, localTestingMat._13, localTestingMat._14 };
			float textMatRow2[4] = { localTestingMat._21, localTestingMat._22, localTestingMat._23, localTestingMat._24 };
			float textMatRow3[4] = { localTestingMat._31, localTestingMat._32, localTestingMat._33, localTestingMat._34 };
			float textMatRow4[4] = { localTestingMat._41, localTestingMat._42, localTestingMat._43, localTestingMat._44 };*/

			ImGui::DragFloat4("row1: ", localTestingMat.m[0], 0.01F);
			ImGui::DragFloat4("row2: ", localTestingMat.m[1], 0.01F);
			ImGui::DragFloat4("row3: ", localTestingMat.m[2], 0.01F);
			ImGui::DragFloat4("row4: ", localTestingMat.m[3], 0.01F);

			

			if (nvgComboKeyIndex != -1 && nvgMainKeyIndex != -1 && ImGui ::TreeNode("Key Binding")) {
				nvgComboKeyIndex = sdh->comboNVKey + 1;
				nvgMainKeyIndex = sdh->nvKey + 1;
				guiKeyIndex = sdh->guiKey + 1;

				if (ImGui::Combo("GUI Key", &guiKeyIndex, mainKey, 117)) {
					sdh->SetGuiKey(guiKeyIndex - 1);
				}

				if (ImGui::Combo("NVG Combo Key", &nvgComboKeyIndex, mainKey, 166)) {
					sdh->SetNVGHotKeyCombo(nvgComboKeyIndex - 1);
				}

				if (ImGui::Combo("NVG Main Key", &nvgMainKeyIndex, mainKey, 166)) {
					sdh->SetNVGHotKeyMain(nvgMainKeyIndex - 1);
				}

				ImGui::TreePop();
				ImGui::NewLine();
			}

			if (ImGui ::TreeNode("Culling Menu")) {
				
				ImGui::SliderInt("countStride", &countStride,0,25564);
				ImGui::SliderInt("countIndexCount", &countIndexCount,0,25564);

				ImGui::TreePop();
				ImGui::NewLine();
			}

			if (ImGui::Checkbox("Edit Mode", &bEnableEditMode)) {

				if (player) {
					bool isInADS = player->gunState == RE::GUN_STATE::kSighted || player->gunState == RE::GUN_STATE::kFireSighted;
					RE::TESIdleForm* sightedForm;

					if (isInADS && !bEnableEditMode) {
						player->SetInIronSightsImpl(false);
						//ended
						sightedForm = (RE::TESIdleForm*)RE::TESForm::GetFormByID(0x4AD9);
						player->currentProcess->PlayIdle(player, 0x35, sightedForm);
					} else if (!isInADS && bEnableEditMode) {
						player->SetInIronSightsImpl(true);
						//started
						sightedForm = (RE::TESIdleForm*)RE::TESForm::GetFormByID(0x4D32);
						player->currentProcess->PlayIdle(player, 0x35, sightedForm);
					}
				}
			}

			if (ImGui::Button("Reload Setting", { 120, 60 })) {
				if (ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData()) {
					ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->ReloadFTSData();
					InitCurrentScopeData(nullptr);
					currData = ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData();
					ResetUIData(currData);
					bReloadRenderMode = true;
				}
			}

			if (bEnableEditMode) {
				if (instance().bRefreshChar) {
					//instance().effectIndex_UI	= ScopeDataHandler::GetSingleton()->GetEffectIndex();
					ResetUIData(currData);
				}

				if (!player)
					return;

				ImGui::NewLine();
				if (ImGui::TreeNode("Main Menu")) {
					ImGui::Checkbox("Disable Culling", &UsingSTS_UI);
					ImGui::Checkbox("Disable Scope Effect While Bolt", &bDisableWhileBolt);
					ImGui::DragInt("Effect Delay(ms)", (int*)&scopeFrame_UI, 1, 0, 5000);
					ImGui::NewLine();
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Shader Data")) {
					ImGui::Checkbox("Circle Scope", &IsCircle_UI);
					ImGui::Checkbox("bEnable ZMove", &bEnableZMove);

					if (bEnableZMove) {
						float currWeaponPos =
							powf(instance().gameConstBuffer.weaponPos.x - instance().gameConstBuffer.rootPos.x, 2) + powf(instance().gameConstBuffer.weaponPos.y - instance().gameConstBuffer.rootPos.y, 2) + powf(instance().gameConstBuffer.weaponPos.z - instance().gameConstBuffer.rootPos.z, 2);
						//powf(instance().weaponPos.z - instance().rootPos.z, 2);

						ImGui::Text("curr WeaponPos_UI %.3f", currWeaponPos);
						ImGui::DragFloat("Base Weapon Pos", &baseWeaponPos_UI, 0.05F);
						ImGui::DragFloat("ZMove Percentage", &MovePercentage_UI, 0.01F, -10, 10);
						ImGui::NewLine();
					}

					ImGui::DragFloat("Scope Depth", &camDepth_UI, 0.01F, 0, 15);
					ImGui::DragFloat("Reticle Size", &ReticleSize_UI, 0.01F, 0, 128);
					ImGui::NewLine();
					ImGui::DragFloat("Min Zoom", &minZoom_UI, 0.01F, 0, 15);
					ImGui::DragFloat("Max Zoom", &maxZoom_UI, 0.01F, 0, 15);
					ImGui::NewLine();
					ImGui::DragFloat2("Dest Pos Offset", PositionOffset_UI, 0.1F, -3840, 3840);
					ImGui::DragFloat2("Dest Scope Size", Size_UI, 0.1F, 0, 3840);
					ImGui::NewLine();
					ImGui::DragFloat2("Ori Pos Offset", OriPositionOffset_UI, 0.1F, -3840, 3840);
					ImGui::DragFloat2("Ori Scope Size", OriSize_UI, 0.1F, 0, 3840);
					ImGui::NewLine();

					ImGui::Checkbox("Enable Night Vision", &bEnableNVGEffect);
					ImGui::DragFloat("Night Vision Intensity", &nvIntensity_UI, 0.1F, 0, 1000);
					ImGui::NewLine();
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Parallax Data")) {
					ImGui::DragFloat("Radius", &radius_UI, 0.01F, 0, 15);
					ImGui::DragFloat("RelativeFog Radius", &relativeFogRadius_UI, 0.01F, 0, 15);
					ImGui::DragFloat("Scope Sway Amount", &scopeSwayAmount_UI, 0.01F, 0, 15);
					ImGui::DragFloat("Max Travel", &maxTravel_UI, 0.01F, 0, 15);
					ImGui::NewLine();
					ImGui::TreePop();
				}

				if (ImGui::Button("Save Setting", { 100, 40 })) {
					currData = ScopeDataHandler::GetSingleton()->GetCurrentFTSData();
					currData->UsingSTS = UsingSTS_UI;
					currData->scopeFrame = scopeFrame_UI;
					currData->shaderData.IsCircle = IsCircle_UI;
					_MESSAGE("currData->shaderData.bCanEnableNV: %i, bEnableNVGEffect: %i", currData->shaderData.bCanEnableNV, bEnableNVGEffect);
					currData->shaderData.bCanEnableNV = bEnableNVGEffect;
					currData->shaderData.baseWeaponPos = baseWeaponPos_UI;
					currData->shaderData.bEnableZMove = bEnableZMove;
					currData->shaderData.movePercentage = MovePercentage_UI;
					currData->shaderData.camDepth = camDepth_UI;
					currData->shaderData.ReticleSize = ReticleSize_UI;
					currData->shaderData.minZoom = minZoom_UI;
					currData->shaderData.maxZoom = maxZoom_UI;
					currData->shaderData.PositionOffset[0] = PositionOffset_UI[0];
					currData->shaderData.PositionOffset[1] = PositionOffset_UI[1];
					currData->shaderData.OriPositionOffset[0] = OriPositionOffset_UI[0];
					currData->shaderData.OriPositionOffset[1] = OriPositionOffset_UI[1];
					currData->shaderData.Size[0] = Size_UI[0];
					currData->shaderData.Size[1] = Size_UI[1];
					currData->shaderData.OriSize[0] = OriSize_UI[0];
					currData->shaderData.OriSize[1] = OriSize_UI[1];
					currData->shaderData.parallax.radius = radius_UI;
					currData->shaderData.parallax.relativeFogRadius = relativeFogRadius_UI;
					currData->shaderData.parallax.scopeSwayAmount = scopeSwayAmount_UI;
					currData->shaderData.parallax.maxTravel = maxTravel_UI;
					currData->shaderData.bBoltDisable = bDisableWhileBolt;
					currData->shaderData.nvIntensity = nvIntensity_UI;

					//UsingSTS_UI = NULL;
					//scopeFrame_UI = NULL;
					//IsCircle_UI = NULL;
					//camDepth_UI = NULL;
					//ReticleSize_UI = NULL;
					//minZoom_UI = NULL;
					//maxZoom_UI = NULL;
					//PositionOffset_UI[0] = NULL;
					//PositionOffset_UI[1] = NULL;
					//OriPositionOffset_UI[0] = NULL;
					//OriPositionOffset_UI[1] = NULL;
					//Size_UI[0] = NULL;
					//Size_UI[1] = NULL;
					//OriSize_UI[0] = NULL;
					//OriSize_UI[1] = NULL;
					//radius_UI = NULL;
					//relativeFogRadius_UI = NULL;
					//scopeSwayAmount_UI = NULL;
					//maxTravel_UI = NULL;
					//bEnableZMove = NULL;
					//baseWeaponPos_UI = NULL;
					//MovePercentage_UI = NULL;
					//bEnableNVGEffect = NULL;
					//bDisableWhileBolt = NULL;
					//nvIntensity_UI = NULL;
					ScopeData::ScopeDataHandler::GetSingleton()->WriteCurrentFTSData();
					ResetUIData(currData);
					instance().bRefreshChar = true;
				}

				{
					instance().scopeData.ScopeEffect_Offset = { PositionOffset_UI[0], PositionOffset_UI[1] };
					instance().scopeData.ScopeEffect_OriPositionOffset = { OriPositionOffset_UI[0], OriPositionOffset_UI[1] };
					instance().scopeData.ScopeEffect_Size = { Size_UI[0], Size_UI[1] };
					instance().scopeData.ScopeEffect_OriSize = { OriSize_UI[0], OriSize_UI[1] };
					instance().scopeData.ReticleSize = ReticleSize_UI;
					instance().scopeData.parallax_Radius = radius_UI;
					instance().scopeData.parallax_relativeFogRadius = relativeFogRadius_UI;
					instance().scopeData.parallax_scopeSwayAmount = scopeSwayAmount_UI;
					instance().scopeData.parallax_maxTravel = maxTravel_UI;

					instance().scopeData.BaseWeaponPos = baseWeaponPos_UI;
					instance().scopeData.MovePercentage = MovePercentage_UI;
					instance().scopeData.EnableZMove = bEnableZMove;
					instance().scopeData.EnableNV = bEnableNVGEffect;

					instance().scopeData.isCircle = IsCircle_UI;
					instance().scopeData.camDepth = camDepth_UI;


					instance().scopeData.nvIntensity = nvIntensity_UI;

					D3D11_MAPPED_SUBRESOURCE mappedDataA;
					HR(g_Context->Map(m_pScopeEffectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataA));
					memcpy_s(mappedDataA.pData, sizeof(ScopeEffectShaderData), &scopeData, sizeof(ScopeEffectShaderData));
					g_Context->Unmap(m_pScopeEffectBuffer.Get(), 0);

					//runtime->set_uniform_value_bool(euv_EnableNightVision, bEnableNVGEffect);
				}
				//else
				//{
				//	ImGui::Text("Can't read FTS data! Please ensure you are in game and have a FTS supported weapon in hand!");
				//}
			}

			auto virDir = instance().scopeData.eyeDirection;
			auto virDirA = instance().gameConstBuffer.virDir;
			auto abvirDir = MatMulNi3(instance().gameConstBuffer.camMat, instance().gameConstBuffer.virDir);

			auto VirDirLerp = instance().scopeData.eyeDirectionLerp;
			auto VirDirLerpA = instance().gameConstBuffer.VirDirLerp;
			auto abVirDirLerp = MatMulNi3(instance().gameConstBuffer.camMat, instance().gameConstBuffer.VirDirLerp);

			auto camMat = instance().gameConstBuffer.camMat;

			ImGui::Text("eyeDirection: %f, %f, %f", virDir.x, virDir.y, virDir.z);
			ImGui::Text("abvirDir: %f, %f, %f", abvirDir.x, abvirDir.y, abvirDir.z);
			ImGui::NewLine();
			ImGui::Text("eyeDirectionLerp: %f, %f, %f", VirDirLerp.x, VirDirLerp.y, VirDirLerp.z);
			ImGui::Text("abVirDirLerp: %f, %f, %f", abVirDirLerp.x, abVirDirLerp.y, abVirDirLerp.z);
			ImGui::NewLine();
			ImGui::Text("camMat R1: %f, %f, %f", gameConstBuffer.camMat.entry[0].pt[0], gameConstBuffer.camMat.entry[0].pt[1], gameConstBuffer.camMat.entry[0].pt[2]);
			ImGui::Text("camMat R2: %f, %f, %f", gameConstBuffer.camMat.entry[1].pt[0], gameConstBuffer.camMat.entry[1].pt[1], gameConstBuffer.camMat.entry[1].pt[2]);
			ImGui::Text("camMat R3: %f, %f, %f", gameConstBuffer.camMat.entry[2].pt[0], gameConstBuffer.camMat.entry[2].pt[1], gameConstBuffer.camMat.entry[2].pt[2]);
			ImGui::End();

			ImGui::Render();
		}

	}
	
	
	void __stdcall D3D::DrawIndexedHook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		bool needToBeCull = false;
		//_MESSAGE("DRAWINDEXED!");
		if (!bSelfDraw && isEnableRender) 
		{
		
			pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
			if (veBuffer != NULL)
				veBuffer->GetDesc(&vedesc);
			if (veBuffer != NULL) {
				veBuffer->Release();
				veBuffer = NULL;
			}

			//get indesc.ByteWidth (comment out if not used)
			pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
			if (inBuffer != NULL)
				inBuffer->GetDesc(&indesc);
			if (inBuffer != NULL) {
				inBuffer->Release();
				inBuffer = NULL;
			}


			//!!!!!WARNING!!!!!!
			//Crash while using Weapons Workbench
		
			if ((20 == Stride && 24 == IndexCount && indesc.ByteWidth == 0x0000000008000000 && vedesc.ByteWidth == 0x0000000008000000 )) 
			{
				pContext->VSGetShader(targetVS.GetAddressOf(), targetVSClassInstance.GetAddressOf(), &targetVSNumClassesInstance);
				pContext->PSGetShader(targetPS.GetAddressOf(), 0, 0);
				pContext->OMGetDepthStencilState(targetDepthStencilState.GetAddressOf(), 0);
				pContext->IAGetInputLayout(targetInputLayout.GetAddressOf());
				pContext->PSGetConstantBuffers(1, 2, &pscBuffer);
				pContext->IAGetIndexBuffer(targetIndexBuffer.GetAddressOf(), &targetIndexBufferFormat, &targetIndexBufferOffset);
				pContext->IAGetVertexBuffers(0, 1, targetVertexBuffer.GetAddressOf(), &targetVertexBufferStrides, &targetVertexBufferOffsets);
				pContext->VSGetConstantBuffers(1, 1, targetVertexConstBuffer.GetAddressOf());
				pContext->VSGetConstantBuffers(2, 1, targetVertexConstBuffer1p5.GetAddressOf());
				pContext->VSGetConstantBuffers(12, 1, targetVertexConstBuffer1.GetAddressOf());
				pContext->PSGetShaderResources(0, 1, DrawIndexedSRV.GetAddressOf());
				
				ID3D11Resource* tempSource = nullptr;
				DrawIndexedSRV->GetResource(&tempSource);
				D3D11_RESOURCE_DIMENSION drd;
				tempSource->GetType(&drd);
				UINT width = 0;
				UINT height = 0;

				if (drd == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
					ID3D11Texture2D* tex2d = nullptr;
					tempSource->QueryInterface(&tex2d);
					D3D11_TEXTURE2D_DESC desc;
					tex2d->GetDesc(&desc);
					width = desc.Width;
					height = desc.Height;
					tex2d->Release();
				} 

				tempSource->Release();

				if (drd == D3D11_RESOURCE_DIMENSION_TEXTURE2D && width == 2048 && height == 2048)
				{
					if (targetVertexConstBuffer.Get() && targetVertexConstBuffer1.Get()) {
						targetVertexConstBuffer.CopyTo(&targetVertexConstBufferOutPut);
						targetVertexConstBuffer1p5.CopyTo(&targetVertexConstBufferOutPut1p5);
						targetVertexConstBuffer1.CopyTo(&targetVertexConstBufferOutPut1);

						D3D11_BUFFER_DESC tempDescA;
						D3D11_BUFFER_DESC tempDescB;
						if (targetVertexConstBufferOutPut.Get() && targetVertexConstBufferOutPut1.Get()) {
							targetVertexConstBufferOutPut->GetDesc(&tempDescA);
							targetVertexConstBuffer1->GetDesc(&tempDescB);
						}
					}

					targetIndexCount = IndexCount;
					targetStartIndexLocation = StartIndexLocation;
					targetBaseVertexLocation = BaseVertexLocation;


					instance().Render();
					instance().RenderToReticleTextureNew(IndexCount, StartIndexLocation, BaseVertexLocation);

					needToBeCull = true;
					
					if (instance().mShaderResourceView)
						instance().mShaderResourceView.ReleaseAndGetAddressOf();
					if (instance().mRTShaderResourceView)
						instance().mRTShaderResourceView.ReleaseAndGetAddressOf();
				}
			}
		}
		

		oldFuncs.drawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);

	}

	inline void** get_vtable_ptr(void* obj)
	{
		return *reinterpret_cast<void***>(obj);
	}

	HRESULT __stdcall D3D::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		//
		void** swapChainVTable = get_vtable_ptr(g_Context.Get());

		bSelfDraw = false;
		if (!isActive_TAA)
		{
			//instance().Render();
		}
		if (isShow) {
			instance().RenderImGui();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		auto thr = oldFuncs.d3dPresent(pSwapChain, SyncInterval, Flags);
		return thr;
	}

	
	//Form PureDark
	#pragma region RenderBeforeUI
	

	struct D3D::ImageSpaceEffectTemporalAA_Render
	{
		static void thunk(uint64_t This, uint64_t a2, uint64_t a3)
		{

			
			func(This, a2, a3);

			bSelfDraw = false;

			if (instance().mShaderResourceView)
				instance().mShaderResourceView.ReleaseAndGetAddressOf();
			if (instance().mRTShaderResourceView)
				instance().mRTShaderResourceView.ReleaseAndGetAddressOf();

		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct D3D::ImageSpaceEffectTemporalAA_IsActive
	{
		static bool thunk(uint64_t a1)
		{
			isActive_TAA = func(a1);
			return isActive_TAA;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct D3D::ImageSpaceEffectBokehDepthOfField_Render
	{
		static void thunk(uint64_t This, uint64_t a2, uint64_t a3)
		{
			func(This, a2, a3);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct D3D::ImageSpaceEffectBokehDepthOfField_IsActive
	{
		static bool thunk(uint64_t a1)
		{

			return func(a1);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

#pragma endregion

	BOOL __stdcall D3D::ClipCursorHook(RECT* lpRect)
	{
		if (isShow && isEnableRender) {
			*lpRect = oldRect;
		}
		return oldFuncs.clipCursor(lpRect);
	}

	void __stdcall D3D::vsSetConstantBuffers(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
	{
		oldFuncs.vsSetConstantBuffers(pContext, StartSlot, NumBuffers, ppConstantBuffers);
	}

	bool D3D::Register() noexcept
	{
		HookImportFunc("d3d11.dll", "D3D11CreateDeviceAndSwapChain", oldFuncs.d3dCreateDevice, (std::uintptr_t)D3D11CreateDeviceAndSwapChainHook);

		HookImportFunc("User32.dll", "ClipCursor", oldFuncs.clipCursor, (std::uintptr_t)ClipCursorHook);

		
		write_vfunc<0x1, ImageSpaceEffectTemporalAA_Render>(RE::VTABLE::ImageSpaceEffectTemporalAA[0].address());
		write_vfunc<0x1, ImageSpaceEffectBokehDepthOfField_Render>(RE::VTABLE::ImageSpaceEffectBokehDepthOfField[0].address());
		write_vfunc<0x8, ImageSpaceEffectTemporalAA_IsActive>(RE::VTABLE::ImageSpaceEffectTemporalAA[0].address());
		write_vfunc<0x8, ImageSpaceEffectBokehDepthOfField_IsActive>(RE::VTABLE::ImageSpaceEffectBokehDepthOfField[0].address());

		return true;
	}

	//Still not working
	void D3D::InstallDrawIndexedHook(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		sdh = ScopeData::ScopeDataHandler::GetSingleton();
		g_Context = context;

		std::uintptr_t* vtbl = (std::uintptr_t*)(g_Context.Get());
		vtbl = (std::uintptr_t*)vtbl[0];
		HookFunc(vtbl, 12, (std::uintptr_t)DrawIndexedHook, (std::uintptr_t*)&oldFuncs.drawIndexed);
	}


	HRESULT __stdcall D3D::D3D11CreateDeviceAndSwapChainHook(IDXGIAdapter* pAdapter,
		D3D_DRIVER_TYPE DriverType,
		HMODULE Software, UINT Flags,
		const D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
		IDXGISwapChain** ppSwapChain,
		ID3D11Device** ppDevice,
		D3D_FEATURE_LEVEL* pFeatureLevel,
		ID3D11DeviceContext** ppImmediateContext)
	{

		D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

		HRESULT ret = oldFuncs.d3dCreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
		if (FAILED(ret)) return ret;

		sdh = ScopeData::ScopeDataHandler::GetSingleton();
		g_Context = *ppImmediateContext;
		g_Device = *ppDevice;
		g_Swapchain = *ppSwapChain;

		std::uintptr_t* vtbl1 = (std::uintptr_t*)(*ppSwapChain);
		vtbl1 = (std::uintptr_t*)vtbl1[0];
		HookFunc(vtbl1, 8, (std::uintptr_t)PresentHook, (std::uintptr_t*)&oldFuncs.d3dPresent);

		std::uintptr_t* vtbl = (std::uintptr_t*)(g_Context.Get());
		vtbl = (std::uintptr_t*)vtbl[0];

		//won't work without a modified dxgi.dll or d3d11.dll
		HookFunc(vtbl, 12, (std::uintptr_t)DrawIndexedHook, (std::uintptr_t*)&oldFuncs.drawIndexed);

		//This one working
		//HookFunc(vtbl, 7, (std::uintptr_t)vsSetConstantBuffers, (std::uintptr_t*)&oldFuncs.vsSetConstantBuffers);

		DXGI_SWAP_CHAIN_DESC sd;
		g_Swapchain->GetDesc(&sd);

		RECT rect;
		auto window = GetActiveWindow();

		oldFuncs.wndProc = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
		SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, (LONG_PTR)WndProcHandler);
		::GetWindowRect(sd.OutputWindow, &oldRect);
		

		if (GetWindowRect(sd.OutputWindow, &rect)) {
			windowWidth = rect.right - rect.left;
			windowHeight = rect.bottom - rect.top;
		}

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		ImGui::StyleColorsDark();

		IMGUI_CHECKVERSION();
		ImGui_ImplWin32_Init(sd.OutputWindow);
		ImGui_ImplDX11_Init((g_Device.Get()), (g_Context.Get()));


		return ret;
	}


	void D3D::ShowMenu(bool flag){isShow = flag;}
	void D3D::QueryChangeReticleTexture() { bChangeAimTexture = true; }
	void D3D::ResetZoomDelta() { bResetZoomDelta = true; }
	void D3D::SetFinishAimAnim(bool flag) { bFinishAimAnim = flag; }
	void D3D::SetScopeEffect(bool flag) { isEnableScopeEffect = flag; }
	bool D3D::GetScopeEffect() { return isEnableScopeEffect; }
	void D3D::SetInterfaceTextRefresh(bool flag) {bRefreshChar = flag;bEnableEditMode = false;}
	void D3D::SetGameConstData(GameConstBuffer c){ gameConstBuffer = c;}
	void D3D::InitPlayerData(RE::PlayerCharacter* pl, RE::PlayerCamera* pc){ player = pl; pcam = pc;}
	void D3D::SetNVG(int flag) { bEnableNVG = flag; }
	void D3D::StartScope(bool flag) { bStartScope = flag; }

	D3D::OldFuncs D3D::oldFuncs;
	bool D3D::isEnableRender = false;
	RECT D3D::oldRect{};
	bool D3D::bStartScope = false;
	bool D3D::bFinishAimAnim = false;
	bool D3D::bRefreshChar = true;
	int D3D::bEnableNVG = 0;
	bool D3D::bQueryRender = false;
	bool D3D::isShow = false;
	bool D3D::bIsUpscaler = false;
}