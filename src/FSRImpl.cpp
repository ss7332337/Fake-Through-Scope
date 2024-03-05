#include "FSRImpl.h"
#include <wrl/client.h>

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

ID3D12Device* pD3D12Device = nullptr;
ID3D12CommandQueue* pD3D12CommandQueue = nullptr;
IDXGIFactory4* pDXGIFactory = nullptr;
IDXGISwapChain1* pDXGISwapChain = nullptr;

char* _MESSAGE(const char* fmt, ...);

#ifndef HR
#	define HR(x)                                               \
		{                                                       \
			HRESULT hr = (x);                                   \
			if (FAILED(hr)) {                                   \
				_MESSAGE("%s, %i, %i", __FILE__, __LINE__, hr); \
			}                                                   \
		}
#endif

void FSRImpl::CreateDevice()
{

}
