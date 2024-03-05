#include <d3d11.h>
#include <detours.h>

// Define a pointer type for the original ID3D11Device::DrawIndexed function
typedef void(STDMETHODCALLTYPE* DrawIndexed_t)(ID3D11DeviceContext* This, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);

// Declare a global variable to store the original ID3D11Device::DrawIndexed function pointer


// Define a proxy class that inherits from ID3D11DeviceContext and overrides the DrawIndexed function
class ProxyDeviceContext : public ID3D11DeviceContext
{
	// Store a pointer to the original ID3D11DeviceContext object
	ID3D11DeviceContext* m_pOriginal;

public:

	static DrawIndexed_t oDrawIndexed;
	// Constructor that takes a pointer to the original ID3D11DeviceContext object
	ProxyDeviceContext(ID3D11DeviceContext* pOriginal) :
		m_pOriginal(pOriginal) {}

	// Override the DrawIndexed function
	void STDMETHODCALLTYPE DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		// Do something before calling the original function, such as logging or modifying the parameters
		printf("DrawIndexed called with IndexCount = %u, StartIndexLocation = %u, BaseVertexLocation = %d\n", IndexCount, StartIndexLocation, BaseVertexLocation);

		// Call the original function with the original or modified parameters
		oDrawIndexed(m_pOriginal, IndexCount, StartIndexLocation, BaseVertexLocation);

		// Do something after calling the original function, such as logging or modifying the results
		printf("DrawIndexed finished\n");
	}

	// Forward all other functions to the original object
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) { return m_pOriginal->QueryInterface(riid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_pOriginal->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_pOriginal->Release(); }
	void STDMETHODCALLTYPE GetDevice(ID3D11Device** ppDevice) { m_pOriginal->GetDevice(ppDevice); }
	HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData) { return m_pOriginal->GetPrivateData(guid, pDataSize, pData); }
	HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID guid, UINT DataSize, const void* pData) { return m_pOriginal->SetPrivateData(guid, DataSize, pData); }
	HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID guid, const IUnknown* pData) { return m_pOriginal->SetPrivateDataInterface(guid, pData); }
	void STDMETHODCALLTYPE VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) { m_pOriginal->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers); }
	void STDMETHODCALLTYPE PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) { m_pOriginal->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews); }
	void STDMETHODCALLTYPE PSSetShader(ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) { m_pOriginal->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances); }
	void STDMETHODCALLTYPE PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) { m_pOriginal->PSSetSamplers(StartSlot, NumSamplers, ppSamplers); }
	void STDMETHODCALLTYPE VSSetShader(ID3D11VertexShader* pVertexShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) { m_pOriginal->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances); }
	void STDMETHODCALLTYPE DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) { m_pOriginal->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation); }
	void STDMETHODCALLTYPE DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) { m_pOriginal->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation); }
	void STDMETHODCALLTYPE GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) { m_pOriginal->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers); }
	void STDMETHODCALLTYPE GSSetShader(ID3D11GeometryShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) { m_pOriginal->GSSetShader(pShader, ppClassInstances, NumClassInstances); }
	void STDMETHODCALLTYPE IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) { m_pOriginal->IASetPrimitiveTopology(Topology); }
	void STDMETHODCALLTYPE VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) { m_pOriginal->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews); }
	void STDMETHODCALLTYPE VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) { m_pOriginal->VSSetSamplers(StartSlot, NumSamplers, ppSamplers); }
	void STDMETHODCALLTYPE Begin(ID3D11Asynchronous* pAsync) { m_pOriginal->Begin(pAsync); }
	void STDMETHODCALLTYPE End(ID3D11Asynchronous* pAsync) { m_pOriginal->End(pAsync); }
	HRESULT STDMETHODCALLTYPE GetData(ID3D11Asynchronous* pAsync, void* pData, UINT DataSize, UINT GetDataFlags) { return m_pOriginal->GetData(pAsync, pData, DataSize, GetDataFlags); }
	void STDMETHODCALLTYPE SetPredication(ID3D11Predicate* pPredicate, BOOL PredicateValue) { m_pOriginal->SetPredication(pPredicate, PredicateValue); }
	void STDMETHODCALLTYPE GSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) { m_pOriginal->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews); }
	void STDMETHODCALLTYPE GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) { m_pOriginal->GSSetSamplers(StartSlot, NumSamplers, ppSamplers); }
	void STDMETHODCALLTYPE OMSetRenderTargets(UINT NumViews,
		ID3D11RenderTargetView* const* ppRenderTargetViews,
		ID3D11DepthStencilView* pDepthStencilView)
	{
		m_pOriginal->OMSetRenderTargets(NumViews,
			ppRenderTargetViews,
			pDepthStencilView);
	}
	void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews(
		UINT NumRTVs,
		ID3D11RenderTargetView* const* ppRenderTargetViews,
		ID3D11DepthStencilView* pDepthStencilView,
		UINT UAVStartSlot,
		UINT NumUAVs,
		ID3D11UnorderedAccessView* const* ppUnorderedAccessViews,
		const UINT* pUAVInitialCounts)
	{
		m_pOriginal->OMSetRenderTargetsAndUnorderedAccessViews(
			NumRTVs,
			ppRenderTargetViews,
			pDepthStencilView,
			UAVStartSlot,
			NumUAVs,
			ppUnorderedAccessViews,
			pUAVInitialCounts);
	}
	void STDMETHODCALLTYPE OMSetBlendState(ID3D11BlendState* pBlendState,
		const FLOAT BlendFactor[4],
		UINT SampleMask)
	{
		m_pOriginal->OMSetBlendState(pBlendState,
			BlendFactor,
			SampleMask);
	}
	void STDMETHODCALLTYPE OMSetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState,
		UINT StencilRef)
	{
		m_pOriginal->OMSetDepthStencilState(pDepthStencilState,
			StencilRef);
	}
	void STDMETHODCALLTYPE SOSetTargets(UINT NumBuffers,
		ID3D11Buffer* const* ppSOTargets,
		const UINT* pOffsets)
	{
		m_pOriginal->SOSetTargets(NumBuffers,
			ppSOTargets,
			pOffsets);
	}
	void STDMETHODCALLTYPE DrawAuto() { m_pOriginal->DrawAuto(); }

	// 通过 ID3D11DeviceContext 继承
	virtual void __stdcall Draw(UINT VertexCount, UINT StartVertexLocation) override;
	virtual HRESULT __stdcall Map(ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource) override;
	virtual void __stdcall Unmap(ID3D11Resource* pResource, UINT Subresource) override;
	virtual void __stdcall PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
	virtual void __stdcall IASetInputLayout(ID3D11InputLayout* pInputLayout) override;
	virtual void __stdcall IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets) override;
	virtual void __stdcall IASetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset) override;
	virtual void __stdcall DrawIndexedInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override;
	virtual void __stdcall DrawInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override;
	virtual void __stdcall Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) override;
	virtual void __stdcall DispatchIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override;
	virtual void __stdcall RSSetState(ID3D11RasterizerState* pRasterizerState) override;
	virtual void __stdcall RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT* pViewports) override;
	virtual void __stdcall RSSetScissorRects(UINT NumRects, const D3D11_RECT* pRects) override;
	virtual void __stdcall CopySubresourceRegion(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox) override;
	virtual void __stdcall CopyResource(ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource) override;
	virtual void __stdcall UpdateSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) override;
	virtual void __stdcall CopyStructureCount(ID3D11Buffer* pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView* pSrcView) override;
	virtual void __stdcall ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]) override;
	virtual void __stdcall ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* pUnorderedAccessView, const UINT Values[4]) override;
	virtual void __stdcall ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* pUnorderedAccessView, const FLOAT Values[4]) override;
	virtual void __stdcall ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil) override;
	virtual void __stdcall GenerateMips(ID3D11ShaderResourceView* pShaderResourceView) override;
	virtual void __stdcall SetResourceMinLOD(ID3D11Resource* pResource, FLOAT MinLOD) override;
	virtual FLOAT __stdcall GetResourceMinLOD(ID3D11Resource* pResource) override;
	virtual void __stdcall ResolveSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) override;
	virtual void __stdcall ExecuteCommandList(ID3D11CommandList* pCommandList, BOOL RestoreContextState) override;
	virtual void __stdcall HSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
	virtual void __stdcall HSSetShader(ID3D11HullShader* pHullShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
	virtual void __stdcall HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
	virtual void __stdcall HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
	virtual void __stdcall DSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
	virtual void __stdcall DSSetShader(ID3D11DomainShader* pDomainShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
	virtual void __stdcall DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
	virtual void __stdcall DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
	virtual void __stdcall CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
	virtual void __stdcall CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts) override;
	virtual void __stdcall CSSetShader(ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
	virtual void __stdcall CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
	virtual void __stdcall CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
	virtual void __stdcall VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
	virtual void __stdcall PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
	virtual void __stdcall PSGetShader(ID3D11PixelShader** ppPixelShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
	virtual void __stdcall PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
	virtual void __stdcall VSGetShader(ID3D11VertexShader** ppVertexShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
	virtual void __stdcall PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
	virtual void __stdcall IAGetInputLayout(ID3D11InputLayout** ppInputLayout) override;
	virtual void __stdcall IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, UINT* pStrides, UINT* pOffsets) override;
	virtual void __stdcall IAGetIndexBuffer(ID3D11Buffer** pIndexBuffer, DXGI_FORMAT* Format, UINT* Offset) override;
	virtual void __stdcall GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
	virtual void __stdcall GSGetShader(ID3D11GeometryShader** ppGeometryShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
	virtual void __stdcall IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY* pTopology) override;
	virtual void __stdcall VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
	virtual void __stdcall VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
	virtual void __stdcall GetPredication(ID3D11Predicate** ppPredicate, BOOL* pPredicateValue) override;
	virtual void __stdcall GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
	virtual void __stdcall GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
	virtual void __stdcall OMGetRenderTargets(UINT NumViews, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView) override;
	virtual void __stdcall OMGetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews) override;
	virtual void __stdcall OMGetBlendState(ID3D11BlendState** ppBlendState, FLOAT BlendFactor[4], UINT* pSampleMask) override;
	virtual void __stdcall OMGetDepthStencilState(ID3D11DepthStencilState** ppDepthStencilState, UINT* pStencilRef) override;
	virtual void __stdcall SOGetTargets(UINT NumBuffers, ID3D11Buffer** ppSOTargets) override;
	virtual void __stdcall RSGetState(ID3D11RasterizerState** ppRasterizerState) override;
	virtual void __stdcall RSGetViewports(UINT* pNumViewports, D3D11_VIEWPORT* pViewports) override;
	virtual void __stdcall RSGetScissorRects(UINT* pNumRects, D3D11_RECT* pRects) override;
	virtual void __stdcall HSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
	virtual void __stdcall HSGetShader(ID3D11HullShader** ppHullShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
	virtual void __stdcall HSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
	virtual void __stdcall HSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
	virtual void __stdcall DSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
	virtual void __stdcall DSGetShader(ID3D11DomainShader** ppDomainShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
	virtual void __stdcall DSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
	virtual void __stdcall DSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
	virtual void __stdcall CSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
	virtual void __stdcall CSGetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews) override;
	virtual void __stdcall CSGetShader(ID3D11ComputeShader** ppComputeShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
	virtual void __stdcall CSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
	virtual void __stdcall CSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
	virtual void __stdcall ClearState(void) override;
	virtual void __stdcall Flush(void) override;
	virtual D3D11_DEVICE_CONTEXT_TYPE __stdcall GetType(void) override;
	virtual UINT __stdcall GetContextFlags(void) override;
	virtual HRESULT __stdcall FinishCommandList(BOOL RestoreDeferredContextState, ID3D11CommandList** ppCommandList) override;
};
