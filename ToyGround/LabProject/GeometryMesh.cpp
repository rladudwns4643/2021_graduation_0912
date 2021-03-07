#include "pch.h"
#include "GeometryMesh.h"

GeometryMesh::GeometryMesh()
{
}

GeometryMesh::~GeometryMesh()
{
}

D3D12_VERTEX_BUFFER_VIEW GeometryMesh::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = VertexByteStride;
	vbv.SizeInBytes = VertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW GeometryMesh::IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = IndexFormat;
	ibv.SizeInBytes = IndexBufferByteSize;

	return ibv;
}

void GeometryMesh::DisposeUploaders()
{
	VertexBufferUploader = nullptr;
	IndexBufferUploader = nullptr;
}
