#include <Windows.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")


DWORD* pSwapChainVtable = NULL;
DWORD* pDeviceContextVTable = NULL;

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
IDXGISwapChain* pSwapChain = NULL;


void* phookD3D11Present = NULL;

bool firstTime = true;
void* detourBuffer = NULL;

void __declspec(naked) PresentNakedHookFunc()
{
	__asm
	{
		push esi;
	}

	if (firstTime)
	{
		__asm
			{
			push eax
			mov eax, [ebp+8]
			mov pSwapChain, eax
			pop eax
			}

		pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
		pDevice->GetImmediateContext(&pContext);


		firstTime = false;
	}

	__asm
	{
		pop esi
		jmp phookD3D11Present
	}
}

const void* __cdecl DetourFunc(BYTE* src, const BYTE* dest, const DWORD length)
{
	BYTE* jump = new BYTE[length + 5];
	detourBuffer = jump;

	DWORD dwVirtualProtectBackup;
	VirtualProtect(src, length, PAGE_READWRITE, &dwVirtualProtectBackup);

	memcpy(jump, src, length);
	jump += length;

	jump[0] = 0xE9;
	*(DWORD*)(jump + 1) = (DWORD)(src + length - jump) - 5;

	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dest - src) - 5;

	VirtualProtect(src, length, dwVirtualProtectBackup, &dwVirtualProtectBackup);

	return jump - length;
}

DWORD __stdcall InitializeHook(LPVOID)
{
	HWND hWnd = GetForegroundWindow();

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP != 0 ? FALSE : TRUE;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext))) {
		MessageBox(hWnd, L"Failed to create directX device and swapchain!", L"uBoos?", MB_ICONERROR);
		return NULL;
	}

	pSwapChainVtable = (DWORD*)pSwapChain;
	pSwapChainVtable = (DWORD*)pSwapChainVtable[0];

	pDeviceContextVTable = (DWORD*)pContext;
	pDeviceContextVTable = (DWORD*)pDeviceContextVTable[0];

	phookD3D11Present = (void*)DetourFunc((BYTE*)(pSwapChainVtable[8] + 0xB), (BYTE*)PresentNakedHookFunc, 5);

	DWORD dwOld;
	VirtualProtect(phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &dwOld);

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();

	return 0;
}

//BOOL __stdcall DllMain(HINSTANCE hInst, DWORD dwReason, void* lpReserved)
//{
//	if (dwReason == DLL_PROCESS_ATTACH) {
//		CreateThread(NULL, 0, InitializeHook, NULL, 0, NULL);
//	} else if (dwReason == DLL_PROCESS_DETACH) {
//		if (detourBuffer) {
//			delete[] detourBuffer;
//		}
//	}
//
//	return TRUE;
//}
