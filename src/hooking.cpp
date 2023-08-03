#include "hooking.h"
#include "DDSTextureLoader11.h"
#include "WICTextureLoader11.h"

#include "MathUtils.h"
#include <vector>
#include <Shlwapi.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>



#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



static const char* current_item = NULL;
static const char* current_itemA = NULL;

bool isInitNVGComboKey = false;
bool isInitNVGMainKey = false;

bool bChangeAimTexture = true;
bool bResetZoomDelta = false;
bool bSelfDraw = false;
bool bGetVSConstBuffer = false;
bool isActive_TAA;

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
const float NEAR_PLANE = 0.1f;    // Near clipping plane distance
const float FAR_PLANE = 1000.0f;  // Far clipping plane distance

long unsigned int gCaptureNum = 0;

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

ID3D11Buffer* targetVertexConstBuffer;
ID3D11Buffer* targetVertexConstBuffer1p5;
ID3D11Buffer* targetVertexConstBuffer1;
			
ID3D11Buffer* targetVertexConstBufferOutPut;
ID3D11Buffer* targetVertexConstBufferOutPut1p5;
ID3D11Buffer* targetVertexConstBufferOutPut1;

ImGuiImpl::ImGuiImplClass* imguiImpl;

struct RTVertex
{
	XMFLOAT3 position;  // 位置
	XMFLOAT2 texcoord;  // 纹理坐标
};

struct CopyCBUFFER
{
public:
	float data[96];  // equivalent to 64 floats
}copyCBUFFER;


XMFLOAT4X4 localTestingMat = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

struct Vertex
{
	float x, y, z;  // 位置
	float r, g, b;  // 颜色
};

// 顶点数据
Vertex gdc_Vertices[] = {
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f},  // 左下角
	{ -1.0f, 3.0f, 0.0f, 0.0f, 2.0f},   // 左上角
	{ 3.0f, -1.0f, 0.0f, 2.0f, 0.0f}    // 右下角
};

unsigned int gdc_Indices[] = {
	0, 1, 2  // 第一个三角形
};

ID3D11Buffer* gdc_pVertexBuffer = NULL;
ID3D11InputLayout* gdc_pVertexLayout = NULL;
ID3D11Buffer* gdc_pIndexBuffer = NULL;
D3D11_INPUT_ELEMENT_DESC gdc_layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 位置元素描述
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },    // 颜色元素描述
};

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
	
	bool legacyFlag = true;

	#define LF(f) (legacyFlag ? (f) : (f) / 1000.0f)

	std::unique_ptr<float[]> LFA(float arr[], size_t size)
	{
		std::unique_ptr<float[]> arrNew(new float[size]);

		for (size_t i = 0; i < size; i++) {
			arrNew[i] = LF(arr[i]);
		}

		return arrNew;
	}

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
						GetSington()->EnableCursor(isShow);
						imguiImpl->PlayerAim(isShow);
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

	XMMATRIX GetProjectionMatrix(float fov)
	{
		if (windowHeight == 0 || windowWidth == 0) {
			windowWidth = RE::BSGraphics::RendererData::GetSingleton()->renderWindow->windowWidth;
			windowHeight = RE::BSGraphics::RendererData::GetSingleton()->renderWindow->windowHeight;
		}

		XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(
			fov,                         // Field of view angle
			windowWidth / windowHeight,  // Aspect ratio
			NEAR_PLANE,                  // Near clipping plane distance
			FAR_PLANE                    // Far clipping plane distance
		);

		return projectionMatrix;
	}

	RE::NiPoint3 D3D::WorldToScreen(RE::NiAVObject* cam, RE::NiAVObject* obj,float fov)
	{
		auto camRot = cam->world.rotate;
		auto camTrans = cam->world.translate;

		auto objRot = obj->world.rotate;
		auto objTrans = obj->world.translate;


		auto worldSpacePoint = camRot * (objTrans - camTrans);

		XMMATRIX projectionMatrix = GetProjectionMatrix(fov);

		XMVECTOR clipPosition = XMVectorSet(worldSpacePoint.x, worldSpacePoint.y, worldSpacePoint.z, 1.0f);
		clipPosition = XMVector4Transform(clipPosition, projectionMatrix);  // Apply projection matrix

		XMVECTOR ndcPosition = clipPosition / clipPosition.m128_f32[3];

		XMVECTOR screenPosition;
		screenPosition.m128_f32[0] = (ndcPosition.m128_f32[0] + 1.0f) / 2.0f * windowWidth;   // X coordinate in pixels
		screenPosition.m128_f32[1] = (1.0f - ndcPosition.m128_f32[1]) / 2.0f * windowHeight;  // Y coordinate in pixels
		screenPosition.m128_f32[2] = ndcPosition.m128_f32[2];                                 // Z coordinate is the depth value

		RE::NiPoint3 outPoint({ screenPosition.m128_f32[0], screenPosition.m128_f32[1], screenPosition.m128_f32[2] });
		return outPoint;
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

		HR(g_Device->CreateInputLayout(gdc_layout, ARRAYSIZE(gdc_layout), blob->GetBufferPointer(), blob->GetBufferSize(), &gdc_pVertexLayout));
		
		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_VS_Output.cso", L"HLSL\\ScopeEffect_VS_Output.hlsl", "main", "vs_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_outPutVertexShader.GetAddressOf()));

		return true;
	}

	void D3D::InitLegacyEffect()
	{
		ComPtr<ID3DBlob> blob;

		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_PS_Legacy.cso", L"HLSL\\ScopeEffect_PS_Legacy.hlsl", "main", "ps_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader_Legacy.GetAddressOf()));

		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_PS_Output_Legacy.cso", L"HLSL\\ScopeEffect_PS_Output_Legacy.hlsl", "main", "ps_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_outPutPixelShader_Legacy.GetAddressOf()));

		HR(CreateShaderFromFile(L"Data\\Shaders\\XiFeiLi\\ScopeEffect_VS_Legacy.cso", L"HLSL\\ScopeEffect_VS_Legacy.hlsl", "main", "vs_5_0", blob.ReleaseAndGetAddressOf()));
		HR(g_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader_Legacy.GetAddressOf()));
	}

	ID3D11BlendState* mTransparentBS = nullptr;

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

		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		
		HR(g_Device->CreateBlendState(&blendDesc, &mTransparentBS));
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

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(gdc_Vertices);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = gdc_Vertices;
		HR(g_Device->CreateBuffer(&bd, &InitData, &gdc_pVertexBuffer));

		// 创建索引缓冲区
		
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * ARRAYSIZE(gdc_Indices);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = gdc_Indices;
		HR(g_Device->CreateBuffer(&bd, &InitData, &gdc_pIndexBuffer));


		D3D11_BUFFER_DESC outputDesc = {};
		outputDesc.ByteWidth = 48;
		outputDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		outputDesc.Usage = D3D11_USAGE_DYNAMIC;
		outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(g_Device->CreateBuffer(&outputDesc, nullptr, &targetVertexConstBufferOutPut));

		outputDesc.ByteWidth = 384;
		HR(g_Device->CreateBuffer(&outputDesc, nullptr, &targetVertexConstBufferOutPut1p5));

		outputDesc.ByteWidth = 752;
		HR(g_Device->CreateBuffer(&outputDesc, nullptr, &targetVertexConstBufferOutPut1));

		CreateBlender();

		return true;
	}

	void D3D::InitInputLayout()
	{

		
	}

	void D3D::OnResize()
	{

		/*m_pRenderTargetView.Reset();
		m_pDepthStencilView.Reset();
		m_pDepthStencilBuffer.Reset();*/

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
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HR(g_Device->CreateTexture2D(&textureDesc, NULL, &mBackBufferCopy));
		HR(g_Device->CreateTexture2D(&textureDesc, NULL, mRTRenderTargetTexture.GetAddressOf()));

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		HR(g_Device->CreateRenderTargetView(mRTRenderTargetTexture.Get(), &renderTargetViewDesc, mRTRenderTargetView.GetAddressOf()));

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
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HR(g_Device->CreateTexture2D(&textureDesc, NULL, mOutPutRTRenderTargetTexture.GetAddressOf()));
		
		HR(g_Device->CreateRenderTargetView(mOutPutRTRenderTargetTexture.Get(), &renderTargetViewDesc, mOutPutRTRenderTargetView.GetAddressOf()));
		HR(g_Device->CreateShaderResourceView(mOutPutRTRenderTargetTexture.Get(), &shaderResourceViewDesc, mOutPutRTShaderResourceView.GetAddressOf()));


		
		D3D11_TEXTURE2D_DESC texDesc = {};
		float outPutWidth = windowWidth;
		float outPutHeight = windowHeight;
		/*float outPutWidth = 4096;
			float outPutHeight = 4096;*/
		DXGI_FORMAT srcFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

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

		HR(g_Device->CreateTexture2D(&texDesc, nullptr, &m_DstTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		HR(g_Device->CreateShaderResourceView(m_DstTexture, &srvDesc, &m_DstView));



		
	}

	IDXGISwapChain3* mSwapChain3;
	bool InitWndHandler = false;

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

	ComPtr<ID3D11ShaderResourceView> DrawIndexedSRV;

	bool bHasDraw = false;
	bool bHasGetBackBuffer = false;
	ComPtr<ID3D11ShaderResourceView> targetSRV;

	void D3D::UpdateScene(FTSData* currData)
	{

		if (bChangeAimTexture) {
			if (GetSington()->mTextDDS_SRV)
					GetSington()->mTextDDS_SRV.ReleaseAndGetAddressOf();

			//g_Context->PSSetShaderResources(5, 1, nullptr);
			const wchar_t* tempPath;
			auto tempWchar = GetWC(currData->ZoomNodePath.c_str());
			tempPath = currData->ZoomNodePath.empty() ? L"Data/Textures/FTS/Empty.dds" : tempWchar;
			HR(CreateDDSTextureFromFile(g_Device.Get(), tempPath, nullptr, GetSington()->mTextDDS_SRV.GetAddressOf()));

			if (tempWchar)
					free((void*)tempWchar);
			bChangeAimTexture = false;

			g_Context->GenerateMips(mTextDDS_SRV.Get());
		}

#pragma region FO4GameConstantBuffer

		if (isEnableScopeEffect) {
			scopeData.EnableMerge = 1;
		} else if (!isEnableScopeEffect) {
			scopeData.ScopeEffect_Zoom = currData->shaderData.minZoom;
			scopeData.EnableMerge = 0;
		}

		float tempCamMat[4][4] = {
			{ gameConstBuffer.camMat.entry[0].pt[0], gameConstBuffer.camMat.entry[0].pt[1], gameConstBuffer.camMat.entry[0].pt[2], 0 },
			{ gameConstBuffer.camMat.entry[1].pt[0], gameConstBuffer.camMat.entry[1].pt[1], gameConstBuffer.camMat.entry[1].pt[2], 0 },
			{ gameConstBuffer.camMat.entry[2].pt[0], gameConstBuffer.camMat.entry[2].pt[1], gameConstBuffer.camMat.entry[2].pt[2], 0 },
			{ 0, 0, 0, 1 }
		};

		memcpy_s(&scopeData.CameraRotation, sizeof(float) * 16, tempCamMat, sizeof(float) * 16);
		//scopeData.CameraRotation = tempCamMat;
		scopeData.CurrRootPos = gameConstBuffer.rootPos.GetXMFLOAT3();
		scopeData.CurrWeaponPos = gameConstBuffer.weaponPos.GetXMFLOAT3();
		scopeData.eyeDirection = gameConstBuffer.virDir.GetXMFLOAT3();
		scopeData.eyeDirectionLerp = { gameConstBuffer.VirDirLerp.x, gameConstBuffer.VirDirLerp.y, gameConstBuffer.VirDirLerp.z };
		scopeData.eyeTranslationLerp = gameConstBuffer.VirTransLerp.GetXMFLOAT3();
		scopeData.FTS_ScreenPos = { gameConstBuffer.ftsScreenPos.x, gameConstBuffer.ftsScreenPos.y };

		
		scopeData.targetAdjustFov = pcam->fovAdjustCurrent;

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

			legacyFlag = currData->legacyMode;

			auto tempPosOffset = LFA(currData->shaderData.PositionOffset, 2);
			auto tempOriPosOffest = LFA(currData->shaderData.OriPositionOffset, 2);

			auto tempSize = LFA(currData->shaderData.Size, 2);
			auto tempSize_Rect = LFA(currData->shaderData.rectSize, 4);

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
			scopeData.ScopeEffect_Offset = { tempPosOffset[0], tempPosOffset[1] };
			scopeData.ScopeEffect_OriPositionOffset = { tempOriPosOffest[0], tempOriPosOffest[1] };
			scopeData.ScopeEffect_OriSize = { currData->shaderData.OriSize[0], currData->shaderData.OriSize[1] };
			scopeData.ScopeEffect_Size = { tempSize[0], tempSize[1] };
			scopeData.rect = { tempSize_Rect[0], tempSize_Rect[1], tempSize_Rect[2], tempSize_Rect[3] };
			scopeData.baseFovAdjustTarget = currData->shaderData.fovAdjust;
			bLegacyMode = currData->legacyMode;

			vsConstanData.CurrRootPos = gameConstBuffer.rootPos.GetXMFLOAT3();
			vsConstanData.CurrWeaponPos = gameConstBuffer.weaponPos.GetXMFLOAT3();
			vsConstanData.eyeDirection = gameConstBuffer.virDir.GetXMFLOAT3();
			vsConstanData.eyeDirectionLerp = gameConstBuffer.VirDirLerp.GetXMFLOAT3();
			vsConstanData.MovePercentage1 = currData->shaderData.movePercentage;
			memcpy_s(&vsConstanData.CameraRotation, sizeof(float) * 16, tempCamMat, sizeof(float) * 16);
		}
#pragma endregion

		D3D11_MAPPED_SUBRESOURCE mappedDataA;
		HR(g_Context->Map(GetSington()->m_pScopeEffectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataA));
		memcpy_s(mappedDataA.pData, sizeof(ScopeEffectShaderData), &GetSington()->scopeData, sizeof(ScopeEffectShaderData));
		g_Context->Unmap(GetSington()->m_pScopeEffectBuffer.Get(), 0);

		D3D11_MAPPED_SUBRESOURCE mappedDataB;
		HR(g_Context->Map(GetSington()->m_VSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataB));
		memcpy_s(mappedDataB.pData, sizeof(VSConstantData), &GetSington()->vsConstanData, sizeof(VSConstantData));
		g_Context->Unmap(GetSington()->m_VSBuffer.Get(), 0);

		constBufferData.width = windowWidth;
		constBufferData.height = windowHeight;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(g_Context->Map(GetSington()->m_pConstantBufferData.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(VSConstantData), &GetSington()->constBufferData, sizeof(VSConstantData));
		g_Context->Unmap(GetSington()->m_pConstantBufferData.Get(), 0);
		
	}

	void D3D::GrabScreen()
	{

		g_Context->OMSetRenderTargets(1, mRTRenderTargetView.GetAddressOf(), tempSV);

		g_Context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		g_Context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

		g_Context->VSSetConstantBuffers(1, 1, &targetVertexConstBufferOutPut);
		g_Context->VSSetConstantBuffers(8, 1, &targetVertexConstBufferOutPut1p5);

		/*mBackBuffer->GetDesc(&bbDesc);
		bbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bbDesc.CPUAccessFlags = 0;
		bbDesc.Usage = D3D11_USAGE_DEFAULT;
		bbDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;*/

		//HR(g_Device->CreateTexture2D(&bbDesc, nullptr, &mDynamicTexture));
		//g_Context->CopyResource(mBackBufferCopy, mBackBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		// 指定图元类型为三角形列表
		g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_Context->IASetInputLayout(gdc_pVertexLayout);
		g_Context->IASetIndexBuffer(gdc_pIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
		g_Context->IASetVertexBuffers(0, 1, &gdc_pVertexBuffer, &stride, &offset);

		g_Context->OMSetBlendState(BSTransparent.Get(), nullptr, 0xFFFFFFFF);
		g_Context->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

		g_Context->PSSetConstantBuffers(4, 1, m_pConstantBufferData.GetAddressOf());
		g_Context->PSSetConstantBuffers(5, 1, m_pScopeEffectBuffer.GetAddressOf());

		g_Context->PSSetShaderResources(4, 1, GetSington()->mShaderResourceView.GetAddressOf());
		g_Context->PSSetShaderResources(5, 1, GetSington()->mTextDDS_SRV.GetAddressOf());

		

		bSelfDraw = true;
		g_Context->DrawIndexed(3, 0, 0);
		bSelfDraw = false;

		//g_Context->OMSetRenderTargets(1, &tempRt, tempSV);
	}

	void D3D::RenderToReticleTexture()
	{
		if (!bIsFirst && isEnableRender)
		{
			g_Context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);

			g_Context->CopyResource(m_DstTexture, mRealBackBuffer);

			g_Context->VSSetShader(m_pVertexShader_Legacy.Get(), nullptr, 0);
			g_Context->PSSetShader(m_pPixelShader_Legacy.Get(), nullptr, 0);
			g_Context->PSSetConstantBuffers(4, 1, m_pConstantBufferData.GetAddressOf());
			g_Context->PSSetConstantBuffers(5, 1, m_pScopeEffectBuffer.GetAddressOf());

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
			g_Context->VSSetConstantBuffers(1, 1, &targetVertexConstBufferOutPut);
			g_Context->VSSetConstantBuffers(2, 1, &targetVertexConstBufferOutPut1p5);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			g_Context->OMSetBlendState(mTransparentBS, nullptr, 0xFFFFFFFF);
			g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_Context->IASetInputLayout(gdc_pVertexLayout);
			g_Context->IASetIndexBuffer(gdc_pIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
			g_Context->IASetVertexBuffers(0, 1, &gdc_pVertexBuffer, &stride, &offset);

			g_Context->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
			g_Context->PSSetShaderResources(4, 1, &m_DstView);
			g_Context->PSSetShaderResources(5, 1, GetSington()->mTextDDS_SRV.GetAddressOf());

			bSelfDraw = true;
			g_Context->DrawIndexed(3, 0, 0);
			bSelfDraw = false;
		}
	}

	void D3D::RenderToReticleTextureNew(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		if (targetVS.Get() && targetVertexConstBufferOutPut)
		{
			g_Context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
                                          
			g_Context->CopyResource(m_DstTexture, mRTRenderTargetTexture.Get());

			g_Context->VSSetShader(targetVS.Get(), targetVSClassInstance.GetAddressOf(), targetVSNumClassesInstance);
			g_Context->PSSetShader(m_outPutPixelShader.Get(), nullptr, 0);
			//g_Context->PSSetShader(m_outPutPixelShader.Get(), nullptr, 0);
			g_Context->PSSetConstantBuffers(4, 1, m_pConstantBufferData.GetAddressOf());
			g_Context->PSSetConstantBuffers(5, 1, m_pScopeEffectBuffer.GetAddressOf());

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

			g_Context->VSSetConstantBuffers(1, 1, &targetVertexConstBufferOutPut);
			g_Context->VSSetConstantBuffers(2, 1, &targetVertexConstBufferOutPut1p5);


			g_Context->IASetIndexBuffer(targetIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, targetIndexBufferOffset);
			g_Context->IASetVertexBuffers(0, 1, targetVertexBuffer.GetAddressOf(), &targetVertexBufferStrides, &targetVertexBufferOffsets);

			g_Context->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
			g_Context->PSSetShaderResources(0, 1, &m_DstView);
			g_Context->PSSetShaderResources(6, 1, &m_DstView);

			bSelfDraw = true;
			g_Context->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
			bSelfDraw = false;
		}
			
	}

	void D3D::MapScopeEffectBuffer(ScopeEffectShaderData data)
	{
		scopeData = data;
	}

	static int copyCount = 0;

	void __stdcall D3D::DrawIndexedHook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		bool needToBeCull = false;

		if (!bSelfDraw) {
			pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
			if (veBuffer != NULL)
				veBuffer->GetDesc(&vedesc);
			SAFE_RELEASE(veBuffer);

			pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);

			if (inBuffer != NULL)
				inBuffer->GetDesc(&indesc);
			SAFE_RELEASE(inBuffer);


			if (20 == Stride && 24 == IndexCount && indesc.ByteWidth == 0x0000000008000000 && vedesc.ByteWidth == 0x0000000008000000) {
				pContext->VSGetShader(targetVS.GetAddressOf(), targetVSClassInstance.GetAddressOf(), &targetVSNumClassesInstance);
				pContext->PSGetShader(targetPS.GetAddressOf(), 0, 0);
				pContext->OMGetDepthStencilState(targetDepthStencilState.GetAddressOf(), 0);
				pContext->IAGetInputLayout(targetInputLayout.GetAddressOf());
				pContext->PSGetConstantBuffers(1, 2, &pscBuffer);
				pContext->IAGetIndexBuffer(targetIndexBuffer.GetAddressOf(), &targetIndexBufferFormat, &targetIndexBufferOffset);
				pContext->IAGetVertexBuffers(0, 1, targetVertexBuffer.GetAddressOf(), &targetVertexBufferStrides, &targetVertexBufferOffsets);

				pContext->VSGetConstantBuffers(1, 1, &targetVertexConstBuffer);
				pContext->VSGetConstantBuffers(2, 1, &targetVertexConstBuffer1p5);
				//pContext->VSGetConstantBuffers(12, 1, targetVertexConstBuffer1.GetAddressOf());

				//targetVertexConstBuffer1->GetDesc(&buffer1Desc);

				pContext->PSGetShaderResources(0, 1, DrawIndexedSRV.GetAddressOf());

				if (!DrawIndexedSRV.Get())
						return oldFuncs.drawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
			
				ID3D11Resource* pResource;
				DrawIndexedSRV->GetResource(&pResource);
				D3D11_SHADER_RESOURCE_VIEW_DESC tempSRVDesc;
				DrawIndexedSRV->GetDesc(&tempSRVDesc);

				// 获取资源的类型
				D3D11_RESOURCE_DIMENSION dimension;
				pResource->GetType(&dimension);

				// 检查是否是 2D 纹理
				if (dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
						// 使用 QueryInterface 获取 ID3D11Texture2D 接口的指针
						ID3D11Texture2D* pTexture2D = nullptr;
						HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture2D);
						if (SUCCEEDED(hr)) {
							// 获取纹理描述
							D3D11_TEXTURE2D_DESC desc;
							pTexture2D->GetDesc(&desc);

							if (desc.Width == 2048 && desc.ArraySize == 1 && tempSRVDesc.Format == DXGI_FORMAT_BC2_UNORM_SRGB && tempSRVDesc.Texture2D.MipLevels == 1 && tempSRVDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D) {
								if (targetVertexConstBuffer 
									&& targetVertexConstBuffer1p5
									//&& targetVertexConstBuffer1
									)
								{

									g_Context->CopyResource(targetVertexConstBufferOutPut, targetVertexConstBuffer);
									g_Context->CopyResource(targetVertexConstBufferOutPut1p5, targetVertexConstBuffer1p5);

								}
	
								targetIndexCount = IndexCount;
								targetStartIndexLocation = StartIndexLocation;
								targetBaseVertexLocation = BaseVertexLocation;

								/*ID3D11DepthStencilView* tempDSV1;
								g_Context->OMGetRenderTargets(1, nullptr, &tempDSV1);
								g_Context->ClearDepthStencilView(tempDSV1, 0, 0, 0);*/

								return g_Context->DrawIndexedInstanced(24, 0, 0, 0, 0);
							}
							SAFE_RELEASE(pTexture2D);
						}
				}
			}
		}

		return oldFuncs.drawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	void D3D::Render()
	{

		if (!g_Swapchain || !g_Device || !g_Context)
		{
			g_Swapchain = RE::BSGraphics::RendererData::GetSingleton()->renderWindow->swapChain;
			g_Device = RE::BSGraphics::RendererData::GetSingleton()->device;
			g_Context = RE::BSGraphics::RendererData::GetSingleton()->context;
		}

		assert(g_Swapchain);
		assert(g_Device);
		assert(g_Context);

		if (bQueryRender) {


			if (!bHasGetBackBuffer) 
			{
				g_Swapchain->QueryInterface(IID_PPV_ARGS(&mSwapChain3));
				
				HR(g_Swapchain->GetBuffer(0, IID_PPV_ARGS(&mBackBuffer)));
				HR(g_Device->CreateRenderTargetView(mBackBuffer, nullptr, m_pRenderTargetView.GetAddressOf()));

				bHasGetBackBuffer = true;
			}

			if (mShaderResourceView)
				mShaderResourceView.ReleaseAndGetAddressOf();

			UINT backBufferIndex = mSwapChain3->GetCurrentBackBufferIndex();
			HR(mSwapChain3->GetBuffer(backBufferIndex, IID_PPV_ARGS(&mRealBackBuffer)));
			HR(g_Device->CreateShaderResourceView(mRealBackBuffer, NULL, mShaderResourceView.GetAddressOf()));


			if (isEnableRender && pcam && player)
			{
				if (bIsFirst) 
				{
					OnResize();
					InitEffect();
					InitLegacyEffect();
					InitResource();
					bIsFirst = false;
				}

				auto currData = sdh->GetCurrentFTSData();

				if (!currData || !currData->containAlladditionalKeywords)
					return;

				g_Context->OMGetRenderTargets(1, &tempRt, &tempSV);

				UpdateScene(currData);
				GetSington()->GrabScreen();

				if (bLegacyMode)
					GetSington()->RenderToReticleTexture();
				else
					GetSington()->RenderToReticleTextureNew(targetIndexCount, targetStartIndexLocation, targetBaseVertexLocation);
				
				g_Context->OMSetRenderTargets(1, &tempRt, tempSV);

				bHasDraw = true;
			}
		}
	}

	
	void D3D::RenderImGui()
	{
		imguiImpl->RenderImgui();
	}

	inline void** get_vtable_ptr(void* obj)
	{
		return *reinterpret_cast<void***>(obj);
	}

	HRESULT __stdcall D3D::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{

		if (!InitWndHandler) {
			g_Swapchain->QueryInterface(IID_PPV_ARGS(&mSwapChain3));
			DXGI_SWAP_CHAIN_DESC sd;
			mSwapChain3->GetDesc(&sd);
			oldFuncs.wndProc = (WNDPROC)SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, (LONG_PTR)WndProcHandler);

			InitWndHandler = true;
		}

		bSelfDraw = false;
		if (isShow) {
			GetSington()->RenderImGui();
			if (ImGui::GetDrawData())
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			else
			{
				g_Swapchain->QueryInterface(IID_PPV_ARGS(&mSwapChain3));
				DXGI_SWAP_CHAIN_DESC sd;
				mSwapChain3->GetDesc(&sd);
				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO();
				(void)io;

				ImGui::StyleColorsDark();

				IMGUI_CHECKVERSION();
				ImGui_ImplWin32_Init(sd.OutputWindow);
				ImGui_ImplDX11_Init((g_Device.Get()), (g_Context.Get()));
			}
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
			GetSington()->Render();

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

	struct D3D::ImageSpaceEffectMotionBlur
	{
		static bool thunk(uint64_t a1)
		{
			// may be is_Active
			//_MESSAGE("ImageSpaceEffectMotionBlur,%i",a1);
			return func(a1);
			//return false;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct D3D::ImageSpaceEffectMotionBlur1
	{
		static INT64 thunk(INT64 This, INT64 a1)
		{
			//_MESSAGE("ImageSpaceEffectMotionBlur 1");
			//return func(This,a1);
			return 0;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct D3D::ImageSpaceEffectMotionBlur2
	{
		static void thunk(RE::TESImageSpaceModifier* imod, float strength, RE::NiAVObject* target)
		{
			//_MESSAGE("ImageSpaceEffectMotionBlur 2");
			/*if (imod->motionBlurStrengthInterpolator)
				return;
			else*/
			return func(imod, strength, target);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct D3D::ImageSpaceEffectMotionBlur3
	{
		static char thunk(INT64 This, INT64 a1, int a2, int a3)
		{
			//_MESSAGE("ImageSpaceEffectMotionBlur 3");
			
			//return func(This,a1,a2,a3);
			return NULL;
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


	D3D* D3D::GetSington()
	{
		static D3D instance;	
		return &instance;
	}

	bool D3D::Register() noexcept
	{

		HookImportFunc("d3d11.dll", "D3D11CreateDeviceAndSwapChain", oldFuncs.d3dCreateDevice, (std::uintptr_t)D3D11CreateDeviceAndSwapChainHook);


		HookImportFunc("User32.dll", "ClipCursor", oldFuncs.clipCursor, (std::uintptr_t)ClipCursorHook);

		
		write_vfunc<0x1, ImageSpaceEffectTemporalAA_Render>(RE::VTABLE::ImageSpaceEffectTemporalAA[0].address());
		write_vfunc<0x1, ImageSpaceEffectBokehDepthOfField_Render>(RE::VTABLE::ImageSpaceEffectBokehDepthOfField[0].address());
		write_vfunc<0x8, ImageSpaceEffectTemporalAA_IsActive>(RE::VTABLE::ImageSpaceEffectTemporalAA[0].address());
		write_vfunc<0x8, ImageSpaceEffectBokehDepthOfField_IsActive>(RE::VTABLE::ImageSpaceEffectBokehDepthOfField[0].address());

		//Disable ImageSpaceEffectMotionBlur tempory
		write_vfunc<0x8, ImageSpaceEffectBokehDepthOfField_IsActive>(RE::VTABLE::ImageSpaceEffectMotionBlur[0].address());

		//write_vfunc<0x8, ImageSpaceEffectMotionBlur>(RE::VTABLE::ImageSpaceEffectMotionBlur[0].address());
		//write_vfunc<0x3, ImageSpaceEffectMotionBlur1>(RE::VTABLE::ImageSpaceEffectMotionBlur[0].address());
		//write_vfunc<0x1, ImageSpaceEffectMotionBlur2>(RE::VTABLE::ImageSpaceEffectMotionBlur[0].address());
		//write_vfunc<0x9, ImageSpaceEffectMotionBlur3>(RE::VTABLE::ImageSpaceEffectMotionBlur[0].address());

		return true;
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

		std::uintptr_t* vtbl2 = (std::uintptr_t*)(*ppDevice);
		vtbl2 = (std::uintptr_t*)vtbl2[0];

		std::uintptr_t* vtbl = (std::uintptr_t*)(g_Context.Get());
		vtbl = (std::uintptr_t*)vtbl[0];

		//won't work without a modified dxgi.dll or d3d11.dll
		HookFunc(vtbl, 12, (std::uintptr_t)DrawIndexedHook, (std::uintptr_t*)&oldFuncs.drawIndexed);

		DXGI_SWAP_CHAIN_DESC sd;
		g_Swapchain->GetDesc(&sd);
		IDXGISwapChain3* mSwapChain;
		g_Swapchain->QueryInterface(IID_PPV_ARGS(&mSwapChain));
		UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		HR(mSwapChain->GetBuffer(backBufferIndex, IID_PPV_ARGS(&GetSington()->mRealBackBuffer)));

		D3D11_TEXTURE2D_DESC realTexDesc;

		GetSington()->mRealBackBuffer->GetDesc(&realTexDesc);
		::GetWindowRect(sd.OutputWindow, &oldRect);

		/////////////////////

		/*windowWidth = RE::BSGraphics::RendererData::GetSingleton()->renderWindow->windowWidth;
		windowHeight = RE::BSGraphics::RendererData::GetSingleton()->renderWindow->windowHeight;*/

		windowWidth = realTexDesc.Width;
		windowHeight = realTexDesc.Height;

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		ImGui::StyleColorsDark();

		IMGUI_CHECKVERSION();
		ImGui_ImplWin32_Init(sd.OutputWindow);
		ImGui_ImplDX11_Init((g_Device.Get()), (g_Context.Get()));

		return ret;
	}


	void ClipTexture()
	{
		//float srcAspect = (float)srcWidth / (float)srcHeight;
		//float dstAspect = (float)texDesc.Width / (float)texDesc.Height;

		//UINT copyWidth, copyHeight;
		//if (srcAspect > dstAspect) {
		//		copyWidth = (UINT)(dstAspect * (float)srcHeight);
		//		copyHeight = srcHeight;
		//} else if (srcAspect < dstAspect) {
		//		copyWidth = srcWidth;
		//		copyHeight = (UINT)((float)srcWidth / dstAspect);
		//} else {
		//		copyWidth = srcWidth;
		//		copyHeight = srcHeight;
		//}

		//D3D11_BOX srcBox = {};
		//UINT srcCenterX = srcWidth / 2;
		//UINT srcCenterY = srcHeight / 2;
		///*UINT srcCenterX = srcWidth;
		//UINT srcCenterY = srcHeight;*/

		//srcBox.left = srcCenterX - copyWidth / 2;
		//srcBox.right = srcCenterX + copyWidth / 2;
		//srcBox.top = srcCenterY - copyHeight / 2;
		//srcBox.bottom = srcCenterY + copyHeight / 2;
		//srcBox.front = 0;                             // must be zero
		//srcBox.back = 1;                              // muse be one

		//UINT dstCenterX = texDesc.Width / 2;
		//UINT dstCenterY = texDesc.Height / 2;

		//copyWidth = min(copyWidth, texDesc.Width);
		//copyHeight = min(copyHeight, texDesc.Height);

		//UINT dstX = dstCenterX - copyWidth / 2;
		//UINT dstY = dstCenterY - copyHeight / 2;

		//dstX = max(dstX, 0);
		//dstY = max(dstY, 0);

		//g_Context->CopySubresourceRegion(pDstTexture, dstSubresource, dstX, dstY, dstZ, mRTRenderTargetTexture.Get(), srcSubresource, &srcBox);
	}

	void D3D::ShowMenu(bool flag){isShow = flag;}
	void D3D::QueryChangeReticleTexture() { bChangeAimTexture = true; }
	void D3D::ResetZoomDelta() { bResetZoomDelta = true; }
	void D3D::SetFinishAimAnim(bool flag) { bFinishAimAnim = flag; }
	void D3D::SetScopeEffect(bool flag) { isEnableScopeEffect = flag; }
	bool D3D::GetScopeEffect() { return isEnableScopeEffect; }
	void D3D::SetInterfaceTextRefresh(bool flag)
	{
		bRefreshChar = flag;
		bEnableEditMode = false;
	}

	void D3D::SetImGuiImplClass(ImGuiImpl::ImGuiImplClass* imguiImplClass)
	{
		imguiImpl = imguiImplClass;
	}


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
	bool D3D::bIsInGame = false;
	bool D3D::isEnableScopeEffect = false;
	bool D3D::bEnableEditMode = false;
	bool D3D::bLegacyMode;

	std::once_flag D3D::flagOnce;
}
