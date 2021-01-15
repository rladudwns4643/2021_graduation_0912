#include "stdafx.h"

// 버퍼 리소스를 생성하는 함수
// 버퍼의 힙 유형에 따라 버퍼 리소스를 생성하고 초기화 데이터가 있으면 초기화한다
ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType,
	D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	ID3D12Resource* pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;	// 정점 버퍼의 크기(바이트)
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void**)&pd3dBuffer);

	if (pData)			// Pipeline02 PPt 12p, Map: GPU에서 CPU로 리소스 읽어옴, UnMap: CPU가 수정한 리소스를 GPU로 전달
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:	// Map을 할 수 없기 때문에 임시로 업로드 버퍼를 생성한 후에 데이터를 수정한다.
		{
			if (ppd3dUploadBuffer)
			{
				// 업로드 버퍼를 생성
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc,
					D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

				// 업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				// 업로드 버퍼의 내용을 디폴트 버퍼에 복사
				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);	// GPU에서 리소스 전체를 복사함 

				// 리소스 배리어를 사용
				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;	// 목표 리소스
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:	// 한번 Map을 실행한 후에 영구적으로 매핑(데이터 수정)을 할 수 있다.(UnMap 필요 없음)
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);	// 업로드 버퍼를 리턴 - 그래야 CPU에서 수정이 가능하기 때문
}