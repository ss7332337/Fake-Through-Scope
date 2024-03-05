#include "D3DProxy.h"


DrawIndexed_t ProxyDeviceContext::oDrawIndexed = NULL;

void __stdcall ProxyDeviceContext::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	m_pOriginal->Draw(VertexCount, StartVertexLocation);
}

HRESULT __stdcall ProxyDeviceContext::Map(ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource)
{
	return m_pOriginal->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
}

void __stdcall ProxyDeviceContext::Unmap(ID3D11Resource* pResource, UINT Subresource)
{
	m_pOriginal->Unmap(pResource, Subresource);
}

void __stdcall ProxyDeviceContext::PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
	m_pOriginal->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::IASetInputLayout(ID3D11InputLayout* pInputLayout)
{
	m_pOriginal->IASetInputLayout(pInputLayout);
}

void __stdcall ProxyDeviceContext::IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
{
	m_pOriginal->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void __stdcall ProxyDeviceContext::IASetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
	m_pOriginal->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}

void __stdcall ProxyDeviceContext::DrawIndexedInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
	m_pOriginal->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void __stdcall ProxyDeviceContext::DrawInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
	m_pOriginal->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void __stdcall ProxyDeviceContext::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
	m_pOriginal->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void __stdcall ProxyDeviceContext::DispatchIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
	m_pOriginal->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void __stdcall ProxyDeviceContext::RSSetState(ID3D11RasterizerState* pRasterizerState)
{
	m_pOriginal->RSSetState(pRasterizerState);
}

void __stdcall ProxyDeviceContext::RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT* pViewports)
{
	m_pOriginal->RSSetViewports(NumViewports, pViewports);
}

void __stdcall ProxyDeviceContext::RSSetScissorRects(UINT NumRects, const D3D11_RECT* pRects)
{
	m_pOriginal->RSSetScissorRects(NumRects, pRects);
}

void __stdcall ProxyDeviceContext::CopySubresourceRegion(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox)
{
	m_pOriginal->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}

void __stdcall ProxyDeviceContext::CopyResource(ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource)
{
	m_pOriginal->CopyResource(pDstResource, pSrcResource);
}

void __stdcall ProxyDeviceContext::UpdateSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch)
{
	m_pOriginal->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}

void __stdcall ProxyDeviceContext::CopyStructureCount(ID3D11Buffer* pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView* pSrcView)
{
	m_pOriginal->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
}

void __stdcall ProxyDeviceContext::ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4])
{
	m_pOriginal->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}

void __stdcall ProxyDeviceContext::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* pUnorderedAccessView, const UINT Values[4])
{
	m_pOriginal->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
}

void __stdcall ProxyDeviceContext::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* pUnorderedAccessView, const FLOAT Values[4])
{
	m_pOriginal->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
}

void __stdcall ProxyDeviceContext::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
{
	m_pOriginal->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}

void __stdcall ProxyDeviceContext::GenerateMips(ID3D11ShaderResourceView* pShaderResourceView)
{
	m_pOriginal->GenerateMips(pShaderResourceView);
}

void __stdcall ProxyDeviceContext::SetResourceMinLOD(ID3D11Resource* pResource, FLOAT MinLOD)
{
	m_pOriginal->SetResourceMinLOD(pResource, MinLOD);
}

FLOAT __stdcall ProxyDeviceContext::GetResourceMinLOD(ID3D11Resource* pResource)
{
	return m_pOriginal->GetResourceMinLOD(pResource);
}

void __stdcall ProxyDeviceContext::ResolveSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
{
	m_pOriginal->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}

void __stdcall ProxyDeviceContext::ExecuteCommandList(ID3D11CommandList* pCommandList, BOOL RestoreContextState)
{
	m_pOriginal->ExecuteCommandList(pCommandList, RestoreContextState);
}

void __stdcall ProxyDeviceContext::HSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
	m_pOriginal->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::HSSetShader(ID3D11HullShader* pHullShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
	m_pOriginal->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
}

void __stdcall ProxyDeviceContext::HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
	m_pOriginal->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
	m_pOriginal->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::DSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
	m_pOriginal->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::DSSetShader(ID3D11DomainShader* pDomainShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
	m_pOriginal->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
}

void __stdcall ProxyDeviceContext::DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
	m_pOriginal->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
	m_pOriginal->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
	m_pOriginal->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts)
{
	m_pOriginal->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}

void __stdcall ProxyDeviceContext::CSSetShader(ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
	m_pOriginal->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
}

void __stdcall ProxyDeviceContext::CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
	m_pOriginal->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
	m_pOriginal->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
	m_pOriginal->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	m_pOriginal->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::PSGetShader(ID3D11PixelShader** ppPixelShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
	m_pOriginal->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
}

void __stdcall ProxyDeviceContext::PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
	m_pOriginal->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::VSGetShader(ID3D11VertexShader** ppVertexShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
	m_pOriginal->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
}

void __stdcall ProxyDeviceContext::PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
	m_pOriginal->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::IAGetInputLayout(ID3D11InputLayout** ppInputLayout)
{
	m_pOriginal->IAGetInputLayout(ppInputLayout);
}

void __stdcall ProxyDeviceContext::IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, UINT* pStrides, UINT* pOffsets)
{
	m_pOriginal->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void __stdcall ProxyDeviceContext::IAGetIndexBuffer(ID3D11Buffer** pIndexBuffer, DXGI_FORMAT* Format, UINT* Offset)
{
	m_pOriginal->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}

void __stdcall ProxyDeviceContext::GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
	m_pOriginal->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::GSGetShader(ID3D11GeometryShader** ppGeometryShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
	m_pOriginal->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
}

void __stdcall ProxyDeviceContext::IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY* pTopology)
{
	m_pOriginal->IAGetPrimitiveTopology(pTopology);
}

void __stdcall ProxyDeviceContext::VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	m_pOriginal->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
	m_pOriginal->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::GetPredication(ID3D11Predicate** ppPredicate, BOOL* pPredicateValue)
{
	m_pOriginal->GetPredication(ppPredicate, pPredicateValue);
}

void __stdcall ProxyDeviceContext::GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	m_pOriginal->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
	m_pOriginal->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::OMGetRenderTargets(UINT NumViews, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView)
{
	m_pOriginal->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
}

void __stdcall ProxyDeviceContext::OMGetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews)
{
	m_pOriginal->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
}

void __stdcall ProxyDeviceContext::OMGetBlendState(ID3D11BlendState** ppBlendState, FLOAT BlendFactor[4], UINT* pSampleMask)
{
	m_pOriginal->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}

void __stdcall ProxyDeviceContext::OMGetDepthStencilState(ID3D11DepthStencilState** ppDepthStencilState, UINT* pStencilRef)
{
	m_pOriginal->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}

void __stdcall ProxyDeviceContext::SOGetTargets(UINT NumBuffers, ID3D11Buffer** ppSOTargets)
{
	m_pOriginal->SOGetTargets(NumBuffers, ppSOTargets);
}

void __stdcall ProxyDeviceContext::RSGetState(ID3D11RasterizerState** ppRasterizerState)
{
	m_pOriginal->RSGetState(ppRasterizerState);
}

void __stdcall ProxyDeviceContext::RSGetViewports(UINT* pNumViewports, D3D11_VIEWPORT* pViewports)
{
	m_pOriginal->RSGetViewports(pNumViewports, pViewports);
}

void __stdcall ProxyDeviceContext::RSGetScissorRects(UINT* pNumRects, D3D11_RECT* pRects)
{
	m_pOriginal->RSGetScissorRects(pNumRects, pRects);
}

void __stdcall ProxyDeviceContext::HSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	m_pOriginal->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::HSGetShader(ID3D11HullShader** ppHullShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
	m_pOriginal->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
}

void __stdcall ProxyDeviceContext::HSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
	m_pOriginal->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::HSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
	m_pOriginal->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::DSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	m_pOriginal->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ProxyDeviceContext::DSGetShader(ID3D11DomainShader** ppDomainShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
	m_pOriginal->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
}

void __stdcall ProxyDeviceContext::DSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
	m_pOriginal->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::DSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
	m_pOriginal->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::CSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	m_pOriginal->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);

}

void __stdcall ProxyDeviceContext::CSGetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews)
{
	m_pOriginal->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
}

void __stdcall ProxyDeviceContext::CSGetShader(ID3D11ComputeShader** ppComputeShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
	m_pOriginal->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
}

void __stdcall ProxyDeviceContext::CSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
	m_pOriginal->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void __stdcall ProxyDeviceContext::CSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
	m_pOriginal->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void __stdcall ProxyDeviceContext::ClearState(void)
{
	m_pOriginal->ClearState();
}

void __stdcall ProxyDeviceContext::Flush(void)
{
	m_pOriginal->Flush();
}

D3D11_DEVICE_CONTEXT_TYPE __stdcall ProxyDeviceContext::GetType(void)
{
	return m_pOriginal->GetType();
}

UINT __stdcall ProxyDeviceContext::GetContextFlags(void)
{
	return m_pOriginal->GetContextFlags();
}

HRESULT __stdcall ProxyDeviceContext::FinishCommandList(BOOL RestoreDeferredContextState, ID3D11CommandList** ppCommandList)
{
	return m_pOriginal->FinishCommandList(RestoreDeferredContextState,ppCommandList);
}
