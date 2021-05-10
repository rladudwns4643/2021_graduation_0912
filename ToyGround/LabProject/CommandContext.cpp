#include "pch.h"
#include "CommandContext.h"
#include "GraphicsRenderer.h"
#include "GameObject.h"
#include "ObjectInfo.h"
#include "Timer.h"
#include "Camera.h"
#include "GeometryMesh.h"
#include "SkinnedModelInstance.h"
#include "TOY_GROUND.h"

void GraphicsContext::Initialize()
{
	// Object Resources
	for (int i = 0; i < skinnedObjectCount; ++i)
	{
		m_SkinnedCBs[i] = std::make_unique<UploadBuffer<ShaderResource::SkinnedConstants>>(Core::g_Device.Get(), 1, true);
	}
	PassCB = std::make_unique<UploadBuffer<ShaderResource::PassConstants>>(Core::g_Device.Get(), passCount, true);
	MaterialBuffer = std::make_unique<UploadBuffer<ShaderResource::MaterialData>>(Core::g_Device.Get(), materialCount, false);
}

void GraphicsContext::Release()
{
}

void GraphicsContext::BuildInstanceBuffer(ObjectInfo* objInfo)
{
	m_InstanceBuffers[objInfo->m_Type] = std::make_unique<UploadBuffer<ShaderResource::InstanceData>>(Core::g_Device.Get(), objInfo->m_InstanceCount, false);
}
void GraphicsContext::UpdateInstanceData(ObjectInfo* objInfo, std::vector<GameObject*>& rItems, bool isFrustum)
{
	if (!objInfo) return;

	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	int InstanceCount = 0;
	for (auto& i : info)
	{
		XMMATRIX world = XMLoadFloat4x4(&rItems[i.second]->m_World);
		XMMATRIX texTransform = XMLoadFloat4x4(&rItems[i.second]->m_TexTransform);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
#ifdef FRUSTUM_CULLMODE
		if (isFrustum && !TOY_GROUND::GetApp()->m_Camera->IsInFrustum(invWorld, rItems[i.second]->m_Bounds)) continue;
#endif
		ShaderResource::InstanceData data;
		DirectX::XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
		DirectX::XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
		data.MaterialIndex = rItems[i.second]->m_MaterialIndex;

		// Write the instance data to structured buffer for the visible objects.
		m_InstanceBuffers[objInfo->m_Type]->CopyData(InstanceCount++, data);
	}
}

void GraphicsContext::UpdateInstanceDatas(std::vector<ObjectInfo*>& objInfos, std::vector<GameObject*>& rItems)
{

	for (auto& objInfo : objInfos)
	{
		const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

		int visibleInstanceCount = 0;

		for (auto& i : info)
		{
			XMMATRIX world = XMLoadFloat4x4(&rItems[i.second]->m_World);
			XMMATRIX texTransform = XMLoadFloat4x4(&rItems[i.second]->m_TexTransform);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

			ShaderResource::InstanceData data;
			XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
			data.MaterialIndex = rItems[i.second]->m_MaterialIndex;

			// Write the instance data to structured buffer for the visible objects.
			m_InstanceBuffers[objInfo->m_Type]->CopyData(visibleInstanceCount++, data);
		}
	}
}

void GraphicsContext::UpdateMaterialBuffer(std::unordered_map<std::string, std::unique_ptr<Material>>& materials)
{
	auto currMaterialBuffer = MaterialBuffer.get();
	for (auto& e : materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Material* mat = e.second.get();

		XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

		ShaderResource::MaterialData matData;
		matData.DiffuseAlbedo = mat->DiffuseAlbedo;
		matData.FresnelR0 = mat->FresnelR0;
		matData.Roughness = mat->Roughness;
		XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
		matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

		currMaterialBuffer->CopyData(mat->MatCBIndex, matData);
	}
}

void GraphicsContext::UpdateMainPassCB(Camera& camera, Light* light)
{
	XMMATRIX view = camera.GetView();
	XMMATRIX proj = camera.GetProj();
	XMMATRIX ortho = camera.GetOrtho();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&mMainPassCB.Ortho, XMMatrixTranspose(ortho));

	mMainPassCB.EyePosW = camera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)Core::g_DisplayWidth, (float)Core::g_DisplayHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / Core::g_DisplayWidth, 1.0f / Core::g_DisplayHeight);
	mMainPassCB.NearZ = CAMERA_ZNEAR;
	mMainPassCB.FarZ = CAMERA_ZFAR;
	mMainPassCB.TotalTime = Core::g_GameTimer->GetTotalTime();
	mMainPassCB.DeltaTime = Core::g_GameTimer->GetTimeElapsed();
	mMainPassCB.AmbientLight = { 0.6f, 0.6f, 0.6f, 1.0f };

	// Directional Light
	mMainPassCB.Lights[0].Direction = light->Direction;
	mMainPassCB.Lights[0].Strength = light->Strength;

	auto currPassCB = PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void GraphicsContext::UpdateSkinnedCBs(UINT skinnedCBIndex, SkinnedModelInstance* skinmodelInstance)
{
	//skinmodelInstance->UpdateSkinnedAnimation(0);

	ShaderResource::SkinnedConstants skinnedConstants;
	std::copy(
		std::begin(skinmodelInstance->FinalTransforms),
		std::end(skinmodelInstance->FinalTransforms),
		&skinnedConstants.BoneTransforms[0]);

	m_SkinnedCBs[skinnedCBIndex]->CopyData(0, skinnedConstants);
}

void GraphicsContext::DrawRenderItem(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, int zLayer, bool isFrustum)
{
	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	for (auto& i : info)
	{
		auto ri = rItems[i.second];

		if (ri->m_ZLayer == zLayer)
		{
#ifdef FRUSTUM_CULLMODE
			if (isFrustum)
			{
				XMMATRIX world = XMLoadFloat4x4(&rItems[i.second]->m_World);
				XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
				if (!TOY_GROUND::GetApp()->m_Camera->IsInFrustum(invWorld, rItems[i.second]->m_Bounds)) continue;
			}
#endif

			Core::g_CommandList->IASetVertexBuffers(0, 1, &ri->m_Geo->VertexBufferView());
			Core::g_CommandList->IASetIndexBuffer(&ri->m_Geo->IndexBufferView());
			Core::g_CommandList->IASetPrimitiveTopology(ri->m_PrimitiveType);

			auto instanceBuffer = m_InstanceBuffers[ri->GetMeshName()]->Resource();
			Core::g_CommandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->GetGPUVirtualAddress());

			if (ri->m_SkinnedModelInst != nullptr)
			{
				D3D12_GPU_VIRTUAL_ADDRESS skinnedCBAddress = m_SkinnedCBs[ri->m_SkinnedCBIndex]->Resource()->GetGPUVirtualAddress();
				Core::g_CommandList->SetGraphicsRootConstantBufferView(5, skinnedCBAddress);
			}
			else
			{
				Core::g_CommandList->SetGraphicsRootConstantBufferView(5, 0);
			}

			// instanceCount = info.size
			// info = instance world 행렬을 갖고있는 맵
			Core::g_CommandList->DrawIndexedInstanced(ri->m_IndexCount, info.size(), ri->m_StartIndexLocation, ri->m_BaseVertexLocation, 0);
		}
	}
}

void GraphicsContext::DrawBBox(const std::vector<BBox*>& rItems)
{
	//for (auto& i : rItems.)
	//{
	//	Core::g_CommandList->IASetVertexBuffers(0, 1, i->m_BB->VertexBufferView());
	//	Core::g_CommandList->IASetIndexBuffer(i->m_BB->IndexBufferView());
	//	Core::g_CommandList->IASetPrimitiveTopology(i->m_PrimitiveType);
	//
	//	Core::g_CommandList->SetGraphicsRootConstantBufferView(6, 0);
	//	Core::g_CommandList->DrawIndexedInstanced(i->m_BB->m_IndexCount, info.size(), i->m_BB->m_StartIndexLocation, i->m_BB->m_BaseVertexLocation, 0);
	//}
}

void GraphicsContext::DrawRenderItems(std::vector<ObjectInfo*>& objInfos, const std::vector<GameObject*>& rItems)
{
	for (auto& objInfo : objInfos) {

		const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

		for (auto& i : info)
		{
			auto ri = rItems[i.second];

			Core::g_CommandList->IASetVertexBuffers(0, 1, &ri->m_Geo->VertexBufferView());
			Core::g_CommandList->IASetIndexBuffer(&ri->m_Geo->IndexBufferView());
			Core::g_CommandList->IASetPrimitiveTopology(ri->m_PrimitiveType);

			auto instanceBuffer = m_InstanceBuffers[ri->GetMeshName()]->Resource();
			Core::g_CommandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->GetGPUVirtualAddress());

			if (ri->m_SkinnedModelInst != nullptr)
			{
				D3D12_GPU_VIRTUAL_ADDRESS skinnedCBAddress = m_SkinnedCBs[ri->m_SkinnedCBIndex]->Resource()->GetGPUVirtualAddress();
				Core::g_CommandList->SetGraphicsRootConstantBufferView(5, skinnedCBAddress);
			}
			else
			{
				Core::g_CommandList->SetGraphicsRootConstantBufferView(5, 0);
			}

			Core::g_CommandList->DrawIndexedInstanced(ri->m_IndexCount, info.size(), ri->m_StartIndexLocation, ri->m_BaseVertexLocation, 0);
		}
	}
}

void GraphicsContext::SetPipelineState(ID3D12PipelineState* PSO)
{
	if (m_CurPipelineState == PSO)
		return;

	//m_CurPipelineState = PSO;
	Core::g_CommandList->SetPipelineState(PSO);
}

void GraphicsContext::SetGraphicsRootSignature(ID3D12RootSignature* RootSignature)
{
	if (m_CurGraphicsRootSignature == RootSignature)
		Core::g_CommandList->SetGraphicsRootSignature(m_CurGraphicsRootSignature);
	else
	{
		Core::g_CommandList->SetGraphicsRootSignature(RootSignature);
		m_CurGraphicsRootSignature = RootSignature;
	}
}
