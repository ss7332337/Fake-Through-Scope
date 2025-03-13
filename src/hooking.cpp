#include "hooking.h"
#include "DDSTextureLoader11.h"
#include "WICTextureLoader11.h"
#include <vector>
#include <Shlwapi.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <d3d11_4.h>
#include <d3dcommon.h>
#include <MinHook.h>
#include <REX/W32/COMPTR.hpp>
#include <MathUtils.h>
#include <optional>

#include <renderdoc_app.h>

#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "shlwapi.lib")

// RenderDoc API接口指针
RENDERDOC_API_1_6_0* rdoc_api = nullptr;

// 原始Present函数指针
typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain*, UINT, UINT);
Present_t OriginalPresent = nullptr;

// 加载RenderDoc API
bool LoadRenderDoc()
{
	// 尝试加载 DLL（如果尚未加载）
	HMODULE mod = GetModuleHandleA("renderdoc.dll");
	if (!mod) {
		// 如果未加载，尝试主动加载（可能需要管理员权限）
		mod = LoadLibraryA("renderdoc.dll");
		if (!mod) return false;

		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");

		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&rdoc_api);
		return ret == 1;
	}
	return false;
}

bool EnableDebugPrivilege()
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return false;
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return false;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return GetLastError() == ERROR_SUCCESS;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static constexpr UINT TARGET_STRIDE = 28;
static constexpr UINT TARGET_INDEX_COUNT = 66;
//static constexpr UINT TARGET_INDEX_COUNT = 24;
static constexpr UINT TARGET_BUFFER_SIZE = 0x0000000008000000;
static constexpr UINT TARGET_TEXTURE_WIDTH = 2048;
static constexpr UINT TARGET_TEXTURE_HEIGHT = 2048;
static constexpr DXGI_FORMAT TARGET_TEXTURE_FORMAT = DXGI_FORMAT_BC2_UNORM_SRGB;


ComPtr<IDXGISwapChain> g_Swapchain = nullptr;
ComPtr<ID3D11Device> g_Device = nullptr;
ComPtr<ID3D11DeviceContext> g_Context = nullptr;

ImGuiIO io;

static const char* current_item = NULL;
static const char* current_itemA = NULL;

int appculltestCount = 0;

bool isInitNVGComboKey = false;
bool isInitNVGMainKey = false;

bool bChangeAimTexture = true;
bool bResetZoomDelta = false;
bool bSelfDraw = false;
bool isActive_TAA = false;
bool isActive_DOF = false;

int nvgComboKeyIndex = 0;
int nvgMainKeyIndex = 0;
int guiKeyIndex = 0;

using namespace DirectX;


static HWND g_hWnd;
static HMODULE g_hModule;
static std::once_flag g_isInitialized;

DWORD_PTR* pSwapChainVTable = nullptr;
DWORD_PTR* pDeviceVTable = nullptr;
DWORD_PTR* pDeviceContextVTable = nullptr;

int windowWidth;
int windowHeight;

XMVECTORF32 bgColor = { 0.1f, 0.1f, 0.1f, 0.0f };
const float NEAR_PLANE = 0.1f;    // Near clipping plane distance
const float FAR_PLANE = 1000.0f;  // Far clipping plane distance

float gameZoomDelta = 1;
bool bIsFirst = true;


ID3D11Buffer* targetVertexConstBuffer;
ID3D11Buffer* targetVertexConstBuffer1p5;
ID3D11Buffer* targetVertexConstBuffer1;
			
ID3D11Buffer* targetVertexConstBufferOutPut;
ID3D11Buffer* targetVertexConstBufferOutPut1p5;
ID3D11Buffer* targetVertexConstBufferOutPut1;

// Boolean
BOOL g_bInitialised = false;
bool g_ShowMenu = false;
bool bDrawIndexed = true;

ImGuiImpl::ImGuiImplClass* imguiImpl;

IDXGISwapChain3* mSwapChain3;
bool InitWndHandler = false;

ID3D11RenderTargetView* tempRt;
ID3D11DepthStencilView* tempSV;

bool bHasGrab = false;

UINT targetIndexCount = 0;
UINT targetStartIndexLocation = 0;
UINT targetBaseVertexLocation = 0;
ComPtr<ID3D11VertexShader> targetVS;
ComPtr<ID3D11ClassInstance> targetVSClassInstance;
UINT targetVSNumClassesInstance;

ComPtr<ID3D11PixelShader> targetPS;
ComPtr<ID3D11DepthStencilState> targetDepthStencilState = nullptr;
ComPtr<ID3D11InputLayout> targetInputLayout;

ID3D11Buffer* targetIndexBuffer;
DXGI_FORMAT targetIndexBufferFormat;
UINT targetIndexBufferOffset;

ComPtr<ID3D11Buffer> targetVertexBuffer;
UINT targetVertexBufferStrides;
UINT targetVertexBufferOffsets;

ComPtr<ID3D11ShaderResourceView> DrawIndexedSRV;

ComPtr<ID3D11BlendState> overrideBlendState;


bool bHasDraw = false;
bool bHasGetBackBuffer = false;
Hook::D3D* D3DInstance = Hook::D3D::GetSington();

struct RTVertex
{
	XMFLOAT3 position;  // 位置
	XMFLOAT2 texcoord;  // 纹理坐标
};

struct Vertex
{
	float x, y, z;  // 位置
	float r, g, b;  // 颜色
};

struct BufferInfo
{
	UINT stride;
	UINT offset;
	D3D11_BUFFER_DESC desc;
};


struct HookInfo
{
	UINT index;
	void* hook;
	void** original;
	const char* name;  // 用于日志
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

unsigned int plane_Indices[] = {
	0, 1, 2,
	2, 3, 0,
	1,4,5,
	5,2,1,
	3,2,6,
	6,7,3,
	2,5,8,
	8,6,2
};

ComPtr<ID3D11Buffer> plane_pIndexBuffer = nullptr;
ID3D11Buffer* defaultBuffer = nullptr;
ID3D11Buffer* copyBuffer = nullptr;


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


char tempbuf[8192] = { 0 };
char* _MESSAGE(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t count = sizeof(tempbuf) - 1;
	int len = vsnprintf(tempbuf, sizeof(tempbuf), fmt, args);
	va_end(args);
	if (len > count) {
		tempbuf[count] = '\0';
	}
	spdlog::log(spdlog::level::warn, tempbuf);

	return tempbuf;
}


namespace Hook
{


	RE::PlayerCharacter* player;
	RE::PlayerCamera* pcam;
	ScopeDataHandler* sdh;

	float cachedLeftConstraintPct;    // 3C
	float cachedRightConstraintPct;  // 40
	float cachedTopConstraintPct;     // 44
	float cachedBottomConstraintPct;  // 48
	
	bool legacyFlag = true;

	#define LF(f) (legacyFlag ? (f) : (f) / 1000.0f)

	std::unique_ptr<float[]> LFA(const float arr[], size_t size)
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


	bool isCapturing = false;
	LRESULT __stdcall D3D::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		switch (msg) {
			case WM_KEYDOWN:
				{
					bool isPressed = (lParam & 0x40000000) != 0x0;
					if (!isPressed && wParam == sdh->guiKey) {
						isShow = !isShow;

						auto mc = RE::MenuCursor::GetSingleton();
						REX::W32::ShowCursor(isShow);
						imguiImpl->PlayerAim(isShow);

						auto input = (RE::BSInputDeviceManager::GetSingleton());
						RE::ControlMap::GetSingleton()->ignoreKeyboardMouse = isShow;		
					}

					if (!isPressed&& wParam == VK_HOME) {
						if (rdoc_api) {
							if (!isCapturing) {
								rdoc_api->StartFrameCapture(nullptr, nullptr);
								isCapturing = true;
							} else {
								rdoc_api->EndFrameCapture(nullptr, nullptr);
								isCapturing = false;
							}
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

		io = ImGui::GetIO();

		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return CallWindowProc(oldFuncs.wndProc, hWnd, msg, wParam, lParam);

	}

	static XMMATRIX GetProjectionMatrix(float fov)
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

	void D3D::CreateBlender()
	{
		// 1. Alpha-To-Coverage 混合状态
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HR(g_Device->CreateBlendState(&blendDesc, BSAlphaToCoverage.GetAddressOf()));

		// 2. 透明混合状态 (BSTransparent)
		blendDesc = {};
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		auto& rtDesc = blendDesc.RenderTarget[0];
		rtDesc.BlendEnable = true;
		rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HR(g_Device->CreateBlendState(&blendDesc, BSTransparent.GetAddressOf()));
		// 创建两个相同的混合状态（如果确实需要两个，否则保留一个）
		//HR(g_Device->CreateBlendState(&blendDesc, &mTransparentBS));  // 如果确实需要第三个
	}

	void CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** buffer, UINT byteWidth)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = byteWidth;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(device->CreateBuffer(&desc, nullptr, buffer));
	}

	template <typename T>
	void CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer** buffer, const T* data, UINT count)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(T) * count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = { data };
		HR(device->CreateBuffer(&desc, &initData, buffer));
	}

	template <typename T>
	void CreateIndexBuffer(ID3D11Device* device, ID3D11Buffer** buffer, const T* data, UINT count)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(T) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = { data };
		HR(device->CreateBuffer(&desc, &initData, buffer));
	}


	bool D3D::InitResource()
	{
		// 常量缓冲区
		CreateConstantBuffer(g_Device.Get(), m_pScopeEffectBuffer.GetAddressOf(), sizeof(ScopeEffectShaderData));
		CreateConstantBuffer(g_Device.Get(), m_pConstantBufferData.GetAddressOf(), sizeof(ConstBufferData));
		CreateConstantBuffer(g_Device.Get(), m_VSBuffer.GetAddressOf(), sizeof(VSConstantData));
		CreateConstantBuffer(g_Device.Get(), m_VSOutBuffer.GetAddressOf(), sizeof(VSOutConstantData));

		// 动态常量缓冲区（示例）
		D3D11_BUFFER_DESC outputDesc = {};
		outputDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		outputDesc.Usage = D3D11_USAGE_DYNAMIC;
		outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		outputDesc.ByteWidth = 48;
		HR(g_Device->CreateBuffer(&outputDesc, nullptr, &targetVertexConstBufferOutPut));
		outputDesc.ByteWidth = 384;
		HR(g_Device->CreateBuffer(&outputDesc, nullptr, &targetVertexConstBufferOutPut1p5));
		outputDesc.ByteWidth = 752;
		HR(g_Device->CreateBuffer(&outputDesc, nullptr, &targetVertexConstBufferOutPut1));

		// 顶点/索引缓冲区
		CreateVertexBuffer(g_Device.Get(), &gdc_pVertexBuffer, gdc_Vertices, ARRAYSIZE(gdc_Vertices));
		CreateIndexBuffer(g_Device.Get(), &gdc_pIndexBuffer, gdc_Indices, ARRAYSIZE(gdc_Indices));

		// 采样器状态
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(g_Device->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf()));

		CreateBlender();
		return true;
	}

	void CreateTextureAndViews(
		ID3D11Device* device,UINT width, UINT height, DXGI_FORMAT format,
		ID3D11Texture2D** texture,ID3D11RenderTargetView** rtv = nullptr,ID3D11ShaderResourceView** srv = nullptr)
	{
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		HR(device->CreateTexture2D(&texDesc, nullptr, texture));

		if (rtv) {
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			HR(device->CreateRenderTargetView(*texture, &rtvDesc, rtv));
		}

		if (srv) {
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			HR(device->CreateShaderResourceView(*texture, &srvDesc, srv));
		}
	}

	void D3D::OnResize()
	{
		//------------------------------
		// 1. 创建平面索引缓冲区
		//------------------------------
		CreateIndexBuffer(g_Device.Get(), plane_pIndexBuffer.GetAddressOf(), plane_Indices, ARRAYSIZE(plane_Indices));

		//------------------------------
		// 2. 创建深度模板缓冲区和视图
		//------------------------------
		// 深度模板纹理描述
		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = windowWidth;
		depthStencilDesc.Height = windowHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// 创建深度模板纹理和视图
		HR(g_Device->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf()));
		HR(g_Device->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf()));

		//------------------------------
		// 3. 创建主渲染目标纹理及视图
		//------------------------------
		CreateTextureAndViews(
			g_Device.Get(),
			windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
			mRTRenderTargetTexture.GetAddressOf(),
			mRTRenderTargetView.GetAddressOf(),
			mRTShaderResourceView.GetAddressOf());

		//------------------------------
		// 4. 创建输出渲染目标纹理及视图 (1024x1024)
		//------------------------------
		CreateTextureAndViews(
			g_Device.Get(),
			1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM,
			mOutPutRTRenderTargetTexture.GetAddressOf(),
			mOutPutRTRenderTargetView.GetAddressOf(),
			mOutPutRTShaderResourceView.GetAddressOf());

		//------------------------------
		// 5. 创建目标纹理及SRV（仅绑定到着色器资源）
		//------------------------------
		constexpr DXGI_FORMAT srcFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;  // 只绑定SRV

		// 创建目标纹理
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = windowWidth;
		texDesc.Height = windowHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = srcFormat;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = bindFlags;

		HR(g_Device->CreateTexture2D(&texDesc, nullptr, &m_DstTexture));

		// 创建关联的SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		HR(g_Device->CreateShaderResourceView(m_DstTexture, &srvDesc, &m_DstView));
	}


	void D3D::LoadAimTexture(const std::string& path)
	{
		if (!path.empty())
			return;

		const wchar_t* tempPath = GetWC(path.c_str());
		std::wstring defaultPath = L"Data/Textures/FTS/Empty.dds";

		D3DInstance->mTextDDS_SRV.Reset();
		HR(CreateDDSTextureFromFile(
			g_Device.Get(),
			tempPath ? tempPath : defaultPath.c_str(),
			nullptr,
			D3DInstance->mTextDDS_SRV.ReleaseAndGetAddressOf()));

		if (mTextDDS_SRV.Get()) {
			g_Context->GenerateMips(mTextDDS_SRV.Get());
		}

		if (tempPath) free((void*)tempPath);
	}

	// 通用缓冲区更新模板
	template <typename T>
	void D3D::UpdateConstantBuffer(const ComPtr<ID3D11Buffer>& buffer, const T& data)
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		HR(g_Context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
		memcpy_s(mapped.pData, sizeof(T), &data, sizeof(T));
		g_Context->Unmap(buffer.Get(), 0);
	}

	void D3D::UpdateGameConstants(const GameConstBuffer& src, ScopeEffectShaderData& dst)
	{
		using namespace detail;
		// 矩阵内存直接复制
		memcpy_s(&dst.CameraRotation, sizeof(float[16]), &src.camMat.entry[0], sizeof(float[9]));  // 只复制3x3部分，之前是float16的，注意看看有没有错误

		CopyVector3(dst.CurrRootPos, src.rootPos);
		CopyVector3(dst.CurrWeaponPos, src.weaponPos);
		CopyVector3(dst.eyeDirection, src.virDir);
		CopyVector3(dst.eyeDirectionLerp, src.VirDirLerp);
		CopyVector3(dst.eyeTranslationLerp, src.VirTransLerp);

		// 二维坐标
		dst.FTS_ScreenPos = { src.ftsScreenPos.x, src.ftsScreenPos.y };
	}

	void D3D::UpdateVsConstants(const GameConstBuffer& src, VSConstantData& dst)
	{
		using namespace detail;

		CopyVector3(dst.CurrRootPos, src.rootPos);
		CopyVector3(dst.CurrWeaponPos, src.weaponPos);
		CopyVector3(dst.eyeDirection, src.virDir);
		CopyVector3(dst.eyeDirectionLerp, src.VirDirLerp);

		memcpy_s(&vsConstanData.CameraRotation, sizeof(float[16]), &src.camMat.entry[0], sizeof(float[9]));
	}

	void D3D::UpdateScene(FTSData* currData)
	{
		if (bChangeAimTexture) {
			LoadAimTexture(currData->ZoomNodePath);
			bChangeAimTexture = false;
		}

#pragma region FO4GameConstantBuffer

		scopeData.EnableMerge = isEnableScopeEffect ? 1 : 0;
		if (!isEnableScopeEffect) {
			scopeData.ScopeEffect_Zoom = currData->shaderData.minZoom;
		}

		UpdateGameConstants(gameConstBuffer, scopeData);

		vsConstanData.MovePercentage1 = currData->shaderData.movePercentage;
		scopeData.targetAdjustFov = pcam->fovAdjustCurrent;

		if (bResetZoomDelta) {
			gameZoomDelta = currData->shaderData.minZoom;
			bResetZoomDelta = false;
		}

		gameZoomDelta = std::clamp(gameZoomDelta, currData->shaderData.minZoom, currData->shaderData.maxZoom);

		scopeData.ScopeEffect_Zoom = gameZoomDelta;
		scopeData.GameFov = pcam->firstPersonFOV;

#pragma endregion

#pragma region MyScopeShaderData
		if (!bEnableEditMode) {
			const auto& shaderData = currData->shaderData;
			legacyFlag = currData->legacyMode;

			// 二维参数处理
			const auto ToFloat2 = [](const auto& arr) { return XMFLOAT2(arr[0], arr[1]); };
			const auto ToFloat4 = [](const auto& arr) { return XMFLOAT4(arr[0], arr[1], arr[2], arr[3]); };

			// 基本参数映射
			scopeData.reticle_Offset = ToFloat2(shaderData.reticle_Offset);
			scopeData.BaseWeaponPos = shaderData.baseWeaponPos;
			scopeData.camDepth = shaderData.camDepth;
			scopeData.EnableNV = shaderData.bCanEnableNV ? bEnableNVG : 0;
			scopeData.EnableZMove = shaderData.bEnableZMove;
			scopeData.isCircle = shaderData.IsCircle;
			scopeData.MovePercentage = shaderData.movePercentage;
			scopeData.nvIntensity = shaderData.nvIntensity;
			scopeData.ReticleSize = shaderData.ReticleSize;
			scopeData.ScopeEffect_Offset = ToFloat2(LFA(shaderData.PositionOffset, 2));
			scopeData.ScopeEffect_OriPositionOffset = ToFloat2(LFA(shaderData.OriPositionOffset, 2));
			scopeData.ScopeEffect_OriSize = ToFloat2(shaderData.OriSize);
			scopeData.ScopeEffect_Size = ToFloat2(LFA(shaderData.Size, 2));
			scopeData.rect = ToFloat4(LFA(shaderData.rectSize, 4));

			// 视差参数
			const auto& parallax = shaderData.parallax;
			scopeData.parallax_maxTravel = parallax.maxTravel;
			scopeData.parallax_Radius = parallax.radius;
			scopeData.parallax_relativeFogRadius = parallax.relativeFogRadius;
			scopeData.parallax_scopeSwayAmount = parallax.scopeSwayAmount;
			scopeData.baseFovAdjustTarget = shaderData.fovAdjust;

			// VS常量数据
			vsConstanData.MovePercentage1 = shaderData.movePercentage;
			UpdateVsConstants(gameConstBuffer, vsConstanData);
		}
#pragma endregion

		// 区域5: 统一更新常量缓冲区
		UpdateConstantBuffer(m_pScopeEffectBuffer, scopeData);
		UpdateConstantBuffer(m_VSBuffer, vsConstanData);

		// 特殊常量缓冲区
		constBufferData.width = windowWidth;
		constBufferData.height = windowHeight;
		UpdateConstantBuffer(m_pConstantBufferData, constBufferData);
		
	}

	void DoCullWeapon(bool isCull)
	{
		using namespace RE;
		std::optional<PlayerCharacter*> _player = PlayerCharacter::GetSingleton();
		if (!_player.has_value())
			return;

		NiPointer<NiNode> playerModel = (*_player)->firstPerson3D;
		if (!playerModel)
			return;

		std::optional<NiObject*> weaponBaseObj = playerModel->GetObjectByName("Weapon");
		if (!weaponBaseObj.has_value())
			return;

		NiPointer<NiNode> weaponBaseNode((*weaponBaseObj)->IsNode());
		if (weaponBaseNode && !weaponBaseNode->children.empty()) {
			//weaponBaseNode->CullGeometry(isCull);
			//weaponBaseNode->IsGeometry()->
			weaponBaseNode->SetAppCulled(isCull);
		}
	}


	void D3D::GrabScreen()
	{
		//DoCullWeapon(true);

		g_Context->OMSetRenderTargets(1, mRTRenderTargetView.GetAddressOf(), tempSV);

		g_Context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		g_Context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

		g_Context->VSSetConstantBuffers(1, 1, &targetVertexConstBufferOutPut);
		g_Context->VSSetConstantBuffers(8, 1, &targetVertexConstBufferOutPut1p5);

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

		g_Context->PSSetShaderResources(4, 1, D3DInstance->mShaderResourceView.GetAddressOf());
		g_Context->PSSetShaderResources(5, 1, D3DInstance->mTextDDS_SRV.GetAddressOf());

		

		bSelfDraw = true;
		g_Context->DrawIndexed(3, 0, 0);
		bSelfDraw = false;

		//DoCullWeapon(false);

		g_Context->OMSetRenderTargets(1, &tempRt, tempSV);
	}

	 // 完整的公共渲染状态设置
	void D3D::SetupCommonRenderState(
		ID3D11Resource* sourceTexture, ID3D11VertexShader* vs, ID3D11ClassInstance* const* vsClassInstances, UINT vsClassInstancesCount,
		ID3D11PixelShader* ps, ID3D11InputLayout* inputLayout, ID3D11BlendState* blendState, const std::vector<VSConstantBufferSlot>& vsCBSlots,
		ID3D11Buffer* indexBuffer,DXGI_FORMAT indexFormat,UINT indexOffset,
		ID3D11Buffer* const* vertexBuffers, const UINT* strides, const UINT* offsets, UINT numVertexBuffers)
	{
		// === 1. 深度模板状态配置 ===
		D3D11_DEPTH_STENCIL_DESC tempDSD{};
		D3D11_DEPTH_STENCILOP_DESC tempDSOPD{};
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

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
		HR(g_Device->CreateDepthStencilState(&tempDSD, depthStencilState.GetAddressOf()));

		// === 2. 核心渲染状态设置 ===
		// 设置渲染目标
		g_Context->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);

		// 复制资源（关键修复点）
		if (sourceTexture && m_DstTexture) {
			g_Context->CopyResource(m_DstTexture, sourceTexture);
		}

		// 设置着色器
		g_Context->VSSetShader(vs, vsClassInstances, vsClassInstancesCount);  // 修复类实例传递
		g_Context->PSSetShader(ps, nullptr, 0);

		// 设置公共常量缓冲区
		g_Context->PSSetConstantBuffers(4, 1, m_pConstantBufferData.GetAddressOf());
		g_Context->PSSetConstantBuffers(5, 1, m_pScopeEffectBuffer.GetAddressOf());

		// === 3. 顶点相关设置 ===
		// 设置输入布局
		g_Context->IASetInputLayout(inputLayout);

		// 设置顶点缓冲区（修复指针传递）
		g_Context->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers, strides, offsets);

		// 设置索引缓冲区
		g_Context->IASetIndexBuffer(indexBuffer, indexFormat, indexOffset);

		// === 4. 混合和深度状态 ===
		g_Context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
		g_Context->OMSetDepthStencilState(depthStencilState.Get(), 0);

		// === 5. 着色器资源 ===
		g_Context->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

		// === 6. 顶点着色器常量缓冲区 ===
		for (const auto& slot : vsCBSlots) {
			g_Context->VSSetConstantBuffers(slot.StartSlot, slot.NumBuffers, slot.ppBuffers);
		}

		// === 7. 图元拓扑 ===
		g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void D3D::RenderToReticleTexture()
	{
		if (!bIsFirst && isEnableRender) {
			// 旧版特定参数

			std::vector<VSConstantBufferSlot> vsCBuffersSlots = {
				{ 1, 1, &targetVertexConstBufferOutPut },
				{ 2, 1, &targetVertexConstBufferOutPut1p5 },
			};

			UINT strides = sizeof(Vertex);
			UINT offsets = 0;

			SetupCommonRenderState(mRealBackBuffer, m_pVertexShader_Legacy.Get(), nullptr, 0, m_pPixelShader_Legacy.Get(), gdc_pVertexLayout, BSTransparent.Get(),
				vsCBuffersSlots, gdc_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0, &gdc_pVertexBuffer, &strides, &offsets, 1);

			// 旧版特定资源
			g_Context->PSSetShaderResources(4, 1, &m_DstView);
			g_Context->PSSetShaderResources(5, 1, D3DInstance->mTextDDS_SRV.GetAddressOf());

			bSelfDraw = true;
			g_Context->DrawIndexed(3, 0, 0);
			bSelfDraw = false;
		}
	}

	void D3D::RenderToReticleTextureNew(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		if (targetVS.Get() && targetVertexConstBufferOutPut) {

			std::vector<VSConstantBufferSlot> vsCBuffersSlots = {
				{ 1, 1, &targetVertexConstBufferOutPut },
				{ 2, 1, &targetVertexConstBufferOutPut1p5 },
				{ 12, 1, &targetVertexConstBufferOutPut1 }
			};

			SetupCommonRenderState(
				mRTRenderTargetTexture.Get(), targetVS.Get(), targetVSClassInstance.GetAddressOf(), targetVSNumClassesInstance, m_outPutPixelShader.Get(), targetInputLayout.Get(),
				BSTransparent.Get(), vsCBuffersSlots, targetIndexBuffer, DXGI_FORMAT_R16_UINT, targetIndexBufferOffset, targetVertexBuffer.GetAddressOf(), &targetVertexBufferStrides,
				&targetVertexBufferOffsets, 1);

			// 新版特有资源绑定
			ID3D11ShaderResourceView* nullSRV = nullptr;
			g_Context->PSSetShaderResources(0, 1, &m_DstView);
			//g_Context->PSSetShaderResources(1, 1, &nullSRV);  // 显式清空未使用的槽位
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

	bool IsTargetDrawCall(const BufferInfo& vertexInfo,const BufferInfo& indexInfo,UINT indexCount)
	{
		return vertexInfo.stride == TARGET_STRIDE && indexCount == TARGET_INDEX_COUNT && indexInfo.desc.ByteWidth == TARGET_BUFFER_SIZE && vertexInfo.desc.ByteWidth == TARGET_BUFFER_SIZE;
	}

	bool IsTargetTexture(ID3D11ShaderResourceView* srv)
	{
		if (!srv)
			return false;

		Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
		srv->GetResource(&pResource);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
		if (FAILED(pResource.As(&pTexture2D))) return false;

		D3D11_TEXTURE2D_DESC texDesc;
		pTexture2D->GetDesc(&texDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srv->GetDesc(&srvDesc);

		return texDesc.Width == TARGET_TEXTURE_WIDTH && texDesc.Height == TARGET_TEXTURE_WIDTH && texDesc.Format == TARGET_TEXTURE_FORMAT && srvDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D;
	}

	bool GetBufferInfo(ID3D11DeviceContext* pContext, BufferInfo& outVertexInfo, BufferInfo& outIndexInfo)
	{
		// 获取顶点缓冲区
		Microsoft::WRL::ComPtr<ID3D11Buffer> _veBuffer;
		pContext->IAGetVertexBuffers(0, 1, &_veBuffer, &outVertexInfo.stride, &outVertexInfo.offset);
		if (_veBuffer)
			_veBuffer->GetDesc(&outVertexInfo.desc);

		// 获取索引缓冲区
		Microsoft::WRL::ComPtr<ID3D11Buffer> _inBuffer;
		DXGI_FORMAT _inFormat;
		pContext->IAGetIndexBuffer(&_inBuffer, &_inFormat, &outIndexInfo.offset);
		if (_inBuffer)
			_inBuffer->GetDesc(&outIndexInfo.desc);

		return _veBuffer && _inBuffer;  // 返回是否成功获取
	}

	void CopyConstantBuffers(ID3D11DeviceContext* pContext,ID3D11Buffer* srcBuffers[],ID3D11Buffer* dstBuffers[],size_t count)
	{
		for (size_t i = 0; i < count; ++i) {
			if (srcBuffers[i] && dstBuffers[i]) {
				pContext->CopyResource(dstBuffers[i], srcBuffers[i]);
			}
		}
	}




	void __stdcall D3D::DrawIndexedHook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		bool needToBeCull = false;

		if (bSelfDraw) {
			return oldFuncs.phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
		}

		BufferInfo vertexInfo, indexInfo;
		if (!GetBufferInfo(pContext, vertexInfo, indexInfo)) {
			return oldFuncs.phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
		}

		if (!IsTargetDrawCall(vertexInfo, indexInfo, IndexCount)) {
			return oldFuncs.phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
		}

		pContext->VSGetShader(targetVS.GetAddressOf(), targetVSClassInstance.GetAddressOf(), &targetVSNumClassesInstance);
		pContext->PSGetShader(targetPS.GetAddressOf(), 0, 0);
		pContext->OMGetDepthStencilState(targetDepthStencilState.GetAddressOf(), 0);
		pContext->IAGetInputLayout(targetInputLayout.GetAddressOf());

		pContext->IAGetIndexBuffer(&targetIndexBuffer, &targetIndexBufferFormat, &targetIndexBufferOffset);

		pContext->IAGetVertexBuffers(0, 1, targetVertexBuffer.GetAddressOf(), &targetVertexBufferStrides, &targetVertexBufferOffsets);
		pContext->VSGetConstantBuffers(1, 1, &targetVertexConstBuffer);
		pContext->VSGetConstantBuffers(2, 1, &targetVertexConstBuffer1p5);
		pContext->VSGetConstantBuffers(12, 1, &targetVertexConstBuffer1);

		pContext->PSGetShaderResources(0, 1, DrawIndexedSRV.GetAddressOf());

		if (!DrawIndexedSRV.Get() || !oldFuncs.phookD3D11DrawIndexed) {
			oldFuncs.phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
			return;
		}

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

				if (desc.Width == TARGET_TEXTURE_WIDTH && desc.Height == TARGET_TEXTURE_WIDTH 
					&& desc.ArraySize == 1 && desc.Format == TARGET_TEXTURE_FORMAT && tempSRVDesc.Format == TARGET_TEXTURE_FORMAT 
					&& tempSRVDesc.Texture2D.MipLevels == 1 && tempSRVDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D) {
					if (targetVertexConstBuffer && targetVertexConstBuffer1p5 && targetVertexConstBuffer1) {

						ID3D11Buffer* srcbuffers[] = { targetVertexConstBuffer, targetVertexConstBuffer1p5, targetVertexConstBuffer1 };
						ID3D11Buffer* dstBuffers[] = { targetVertexConstBufferOutPut, targetVertexConstBufferOutPut1p5, targetVertexConstBufferOutPut1 };
						CopyConstantBuffers(pContext, srcbuffers, dstBuffers, 3);
					}

					targetIndexCount = IndexCount;
					targetStartIndexLocation = StartIndexLocation;
					targetBaseVertexLocation = BaseVertexLocation;

					return oldFuncs.phookD3D11DrawIndexed(pContext, 0, 0, 0);
				}
				SAFE_RELEASE(pTexture2D);
			}
		}
	}

	void D3D::Render()
	{

		if (!g_Swapchain.Get() || !g_Device.Get() || !g_Context.Get())
		{
			g_Swapchain = (IDXGISwapChain*)static_cast<void*>(RE::BSGraphics::RendererData::GetSingleton()->renderWindow->swapChain);
			g_Device = (ID3D11Device*)static_cast<void*>(RE::BSGraphics::RendererData::GetSingleton()->device);
			g_Context = (ID3D11DeviceContext*)static_cast<void*>(RE::BSGraphics::RendererData::GetSingleton()->context);
		}

		assert(g_Swapchain.Get());
		assert(g_Device.Get());
		assert(g_Context.Get());

		if (bQueryRender) {


			if (!bHasGetBackBuffer) 
			{
				g_Swapchain->QueryInterface(IID_PPV_ARGS(&mSwapChain3));
				
				HR(g_Swapchain->GetBuffer(0, IID_PPV_ARGS(&mBackBuffer)));
				HR(g_Device->CreateRenderTargetView(mBackBuffer, nullptr, m_pRenderTargetView.GetAddressOf()));

				bHasGetBackBuffer = true;
			}

			if (mShaderResourceView.Get())
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
				D3DInstance->GrabScreen();

				if (legacyFlag) D3DInstance->RenderToReticleTexture();
				else D3DInstance->RenderToReticleTextureNew(targetIndexCount, targetStartIndexLocation, targetBaseVertexLocation);
				
				g_Context->OMSetRenderTargets(1, &tempRt, tempSV);

				bHasDraw = true;
			}
		}
	}

	
	void D3D::RenderImGui()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowMetricsWindow();

		imguiImpl->RenderImgui();

		ImGui::Render();
		ImGui::EndFrame();
	}

	inline void** get_vtable_ptr(void* obj)
	{
		return *reinterpret_cast<void***>(obj);
	}

	HRESULT __fastcall D3D::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		
		if (!imguiImpl)
			return -1;


		bSelfDraw = false;
		imguiImpl->EnableImGuiRender(isShow, true);

		if (isShow) {

			D3DInstance->RenderImGui();
			io.WantCaptureMouse = true;
			if (ImGui::GetDrawData())
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			else
			{
				_MESSAGE("Can't Get DrawData. ReIniting");
			}
		}
		else
		{
			io.WantCaptureMouse = false;
		}

		return oldFuncs.phookD3D11Present(pSwapChain, SyncInterval, Flags);
	}

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
		using namespace REL;
		logger::info("Register Start!");

		return true;
	}

	void __stdcall D3D::hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery)
	{
		if (pQueryDesc->Query == D3D11_QUERY_OCCLUSION) {
			D3D11_QUERY_DESC oqueryDesc = CD3D11_QUERY_DESC();
			(&oqueryDesc)->MiscFlags = pQueryDesc->MiscFlags;
			(&oqueryDesc)->Query = D3D11_QUERY_TIMESTAMP;

			return oldFuncs.phookD3D11CreateQuery(pDevice, &oqueryDesc, ppQuery);
		}

		return oldFuncs.phookD3D11CreateQuery(pDevice, pQueryDesc, ppQuery);
	}

	UINT pssrStartSlot;
	D3D11_SHADER_RESOURCE_VIEW_DESC Descr;

	void __stdcall D3D::hookD3D11PSSetShaderResources(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
	{
		pssrStartSlot = StartSlot;

		for (UINT j = 0; j < NumViews; j++) {
			ID3D11ShaderResourceView* pShaderResView = ppShaderResourceViews[j];
			if (pShaderResView) {
				pShaderResView->GetDesc(&Descr);

				if ((Descr.ViewDimension == D3D11_SRV_DIMENSION_BUFFER) || (Descr.ViewDimension == D3D11_SRV_DIMENSION_BUFFEREX)) {
					continue;  //Skip buffer resources
				}
			}
		}

		return oldFuncs.phookD3D11PSSetShaderResources(pContext, StartSlot, NumViews, ppShaderResourceViews);
	}

	void __stdcall D3D::ClearRenderTargetViewHook(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4])
	{
		return oldFuncs.phookD3D11ClearRenderTargetViewHook(pContext, pRenderTargetView, ColorRGBA);
	}

	bool CreateAndEnableHook(void* target, void* hook, void** original, const char* hookName)
	{
		if (MH_CreateHook(target, hook, original) != MH_OK) {
			logger::error("Failed to create %s hook", hookName);
			return false;
		}
		if (MH_EnableHook(target) != MH_OK) {
			logger::error("Failed to enable %s hook", hookName);
			return false;
		}
		return true;
	}


	void D3D::InitializeImGui(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		swapChain->GetDesc(&sd);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(sd.OutputWindow);
		ImGui_ImplDX11_Init(device, context);
		ImGui::StyleColorsDark();

		oldFuncs.wndProc = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHandler)));
	}

	bool isActive_Blur = false;

	struct HookedRender_TAA
	{
		// thunk 函数
		static void thunk(
			RE::ImageSpaceEffectTemporalAA* This,
			RE::BSTriShape* a_geometry,
			RE::ImageSpaceEffectParam* a_param)
		{
			// 调用原函数
			func.get()(This, a_geometry, a_param);
			isActive_TAA = (This->IsActive());
			if (D3D::isEnableRender && This->IsActive()) {
				D3DInstance->Render();
			}
		}

		// 保存原函数地址的 Relocation 实例
		static inline REL::Relocation<decltype(thunk)*> func;
	};


	RE::TESImageSpaceModifier* BingleTunnelVisionForm;

	
	bool ImageSpaceModifierInstanceFormHookSelfCall = false;


	DWORD __stdcall D3D::HookDX11_Init()
	{
		_MESSAGE("HookDX11_Init");

		const auto rendererData = RE::BSGraphics::RendererData::GetSingleton();
		g_Swapchain = (IDXGISwapChain*)static_cast<void*>(rendererData->renderWindow->swapChain);
		g_Device = (ID3D11Device*)static_cast<void*>(rendererData->device);
		g_Context = (ID3D11DeviceContext*)static_cast<void*>(rendererData->context);

		EnableDebugPrivilege();

		_MESSAGE("Post D3D11CreateDeviceAndSwapChain");

		// 获取虚函数表
		pSwapChainVTable = *reinterpret_cast<DWORD_PTR**>(g_Swapchain.Get());
		pDeviceVTable = *reinterpret_cast<DWORD_PTR**>(g_Device.Get());
		pDeviceContextVTable = *reinterpret_cast<DWORD_PTR**>(g_Context.Get());

		if (!g_bInitialised) {
			_MESSAGE("\t[+] Present Hook called by first time");
			DXGI_SWAP_CHAIN_DESC sd;
			g_Swapchain->GetDesc(&sd);
			::GetWindowRect(sd.OutputWindow, &oldRect);
			sdh = ScopeData::ScopeDataHandler::GetSingleton();
			g_bInitialised = true;
		}

		// 窗口和ImGui初始化
		if (!InitWndHandler) {
			InitializeImGui(g_Swapchain.Get(), g_Device.Get(), g_Context.Get());
			InitWndHandler = true;
		}

		_MESSAGE("Get VTable");

		// 初始化MinHook
		if (MH_Initialize() != MH_OK) {
			_MESSAGE("Failed to initialize MinHook");
			return 1;
		}

		auto taa = RE::VTABLE::ImageSpaceEffectTemporalAA[0];
		// 批量创建钩子
		const std::pair<DWORD_PTR*, HookInfo> hooks[] = {
			{ pSwapChainVTable, HookInfo{ 8, reinterpret_cast<void*>(PresentHook), reinterpret_cast<void**>(&oldFuncs.phookD3D11Present), "PresentHook" } },
			{ pDeviceContextVTable, HookInfo{ 12, reinterpret_cast<void*>(DrawIndexedHook), reinterpret_cast<void**>(&oldFuncs.phookD3D11DrawIndexed), "DrawIndexedHook" } },
			{ pDeviceVTable, HookInfo{ 24, reinterpret_cast<void*>(hookD3D11CreateQuery), reinterpret_cast<void**>(&oldFuncs.phookD3D11CreateQuery), "hookD3D11CreateQuery" } },
			{ pDeviceContextVTable, HookInfo{ 8, reinterpret_cast<void*>(hookD3D11PSSetShaderResources), reinterpret_cast<void**>(&oldFuncs.phookD3D11PSSetShaderResources), "hookD3D11PSSetShaderResources" } },
			{ pSwapChainVTable, HookInfo{ 50, reinterpret_cast<void*>(ClearRenderTargetViewHook), reinterpret_cast<void**>(&oldFuncs.phookD3D11ClearRenderTargetViewHook), "ClearRenderTargetViewHook" } },
		};

		for (const auto& [vtable, info] : hooks) {
			CreateAndEnableHook(reinterpret_cast<void*>(vtable[info.index]), info.hook, info.original, info.name);
		}

		REL::Relocation<std::uintptr_t> vtable_TAA(RE::ImageSpaceEffectTemporalAA::VTABLE[0]);
		const auto oldFuncTAA = vtable_TAA.write_vfunc(1, reinterpret_cast<std::uintptr_t>(&HookedRender_TAA::thunk));
		HookedRender_TAA::func = REL::Relocation<decltype(HookedRender_TAA::thunk)*>(oldFuncTAA);

		_MESSAGE("Install Hook");

		DWORD old_protect;
		VirtualProtect(oldFuncs.phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &old_protect);

		_MESSAGE("VirtualProtect");

		Register();

		return S_OK;
	}

	D3D11_HOOK_API void D3D::ImplHookDX11_Init(HMODULE hModule, void* hwnd)
	{
		g_hWnd = (HWND)hwnd;
		g_hModule = hModule;
		HookDX11_Init();
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
