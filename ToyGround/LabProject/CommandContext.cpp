#include "pch.h"
#include "CommandContext.h"
#include "ApplicationContext.h"
#include "GraphicsRenderer.h"
#include "GameObject.h"
#include "ObjectInfo.h"
#include "Timer.h"
#include "Camera.h"
#include "GeometryMesh.h"
#include "BoundingBoxMesh.h"
#include "SkinnedModelInstance.h"
#include "UserInterface.h"
#include "Particle.h"
#include "TOY_GROUND.h"

void GraphicsContext::Initialize()
{
	// Object Resources
	for (int i = 0; i < skinnedObjectCount; ++i)
	{
		m_SkinnedCBs[i] = std::make_unique<UploadBuffer<ShaderResource::SkinnedConstants>>(Core::g_Device.Get(), 1, true);
	}
	PassCB = std::make_unique<UploadBuffer<ShaderResource::PassConstants>>(Core::g_Device.Get(), passCount, true);
	UIPassCB = std::make_unique<UploadBuffer<ShaderResource::UIPassConstants>>(Core::g_Device.Get(), 1, true);
	MaterialBuffer = std::make_unique<UploadBuffer<ShaderResource::MaterialData>>(Core::g_Device.Get(), materialCount, false);
}

void GraphicsContext::Release()
{
}

void GraphicsContext::BuildInstanceBuffer(ObjectInfo* objInfo)
{
	m_InstanceBuffers[objInfo->m_Type] = std::make_unique<UploadBuffer<ShaderResource::InstanceData>>(Core::g_Device.Get(), objInfo->m_InstanceCount, false);
}
void GraphicsContext::UpdateInstanceData(ObjectInfo* objInfo, std::vector<GameObject*>& rItems, bool isCharacter, bool isParticle)
{
	if (!objInfo) return;

	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	int InstanceCount = 0;
	for (auto& i : info)
	{
		if (rItems[i.second]->m_IsVisible)
		{
			DirectX::XMFLOAT4X4 tWorld = rItems[i.second]->m_World;
			if(isCharacter)
				tWorld._42 -= 10.f;
			XMMATRIX world = XMLoadFloat4x4(&tWorld);
			XMMATRIX texTransform = XMLoadFloat4x4(&rItems[i.second]->m_TexTransform);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
#ifdef FRUSTUM_CULLMODE
			if (rItems[i.second]->m_IsCulling)
			{
				if (!TOY_GROUND::GetApp()->m_Camera->IsInFrustum(invWorld, rItems[i.second]->m_Bounds)) continue;
			}
#endif
			ShaderResource::InstanceData data;
			DirectX::XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
			DirectX::XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
			data.MaterialIndex = rItems[i.second]->m_MaterialIndex;
			if (isParticle)
			{
				Particle* parti = static_cast<Particle*>(rItems[i.second]);
				data.particleTime = parti->GetParticleTotalTime();
				data.particleIsLoop = parti->GetIsLoop();
			}
			else
			{
				data.particleTime = 0.f;
				data.particleIsLoop = false;
			}

			m_InstanceBuffers[objInfo->m_Type]->CopyData(InstanceCount++, data);
		}
	}
}

void GraphicsContext::UpdateGemInstanceData(ObjectInfo* objInfo, std::vector<GameObject*>& rItems)
{
	if (!objInfo) return;

	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	int InstanceCount = 0;
	for (auto& i : info)
	{
		if (rItems[i.second]->m_IsVisible)
		{
			rItems[i.second]->addY += rItems[i.second]->directY;
			if (rItems[i.second]->addY > 19.f)
				rItems[i.second]->directY = -1;
			else if(rItems[i.second]->addY < 1.f)
				rItems[i.second]->directY = 1;
			DirectX::XMFLOAT4X4 tWorld = rItems[i.second]->m_World;
			tWorld._42 += (10.f + rItems[i.second]->addY);
			XMMATRIX world = XMLoadFloat4x4(&tWorld);
			XMMATRIX texTransform = XMLoadFloat4x4(&rItems[i.second]->m_TexTransform);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
#ifdef FRUSTUM_CULLMODE
			if (rItems[i.second]->m_IsCulling)
			{
				if (!TOY_GROUND::GetApp()->m_Camera->IsInFrustum(invWorld, rItems[i.second]->m_Bounds)) continue;
			}
#endif
			ShaderResource::InstanceData data;
			DirectX::XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
			DirectX::XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
			data.MaterialIndex = rItems[i.second]->m_MaterialIndex;
			data.particleTime = 0.f;
			data.particleIsLoop = false;
			m_InstanceBuffers[objInfo->m_Type]->CopyData(InstanceCount++, data);
		}
	}
}
void GraphicsContext::UpdateBulletInstanceData(ObjectInfo* objInfo, std::vector<GameObject*>& rItems)
{
	if (!objInfo) return;

	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	int InstanceCount = 0;
	for (auto& i : info)
	{
		DirectX::XMFLOAT4X4 tWorld = rItems[i.second]->m_World;
		if (rItems[i.second]->m_IsVisible == false)
		{
			tWorld._41 = 0.f;
			tWorld._42 = - 400.f;
			tWorld._43 = 0.f;
		}
		XMMATRIX world = XMLoadFloat4x4(&tWorld);
		XMMATRIX texTransform = XMLoadFloat4x4(&rItems[i.second]->m_TexTransform);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
#ifdef FRUSTUM_CULLMODE
		if (rItems[i.second]->m_IsCulling)
		{
			if (!TOY_GROUND::GetApp()->m_Camera->IsInFrustum(invWorld, rItems[i.second]->m_Bounds)) continue;
		}
#endif
		ShaderResource::InstanceData data;
		DirectX::XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
		DirectX::XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
		data.MaterialIndex = rItems[i.second]->m_MaterialIndex;

		// Write the instance data to structured buffer for the visible objects.
		m_InstanceBuffers[objInfo->m_Type]->CopyData(InstanceCount++, data);
	}
}

void GraphicsContext::Update2DPosition(ObjectInfo* objInfo, std::vector<GameObject*>& rItems)
{
	if (!objInfo) return;

	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	for (auto& i : info)
	{
		UserInterface* ri = static_cast<UserInterface*>(rItems[i.second]);
		switch (ri->GetTextAlignType())
		{
		case TextAlignType::NONE:
			break;
		case TextAlignType::Center:
			if (Core::g_DisplayWidth > Core::g_DisplayHeight)
			{
				float heightRatio = Core::g_DisplayHeight / 1080.f;

				XMFLOAT2 offset = { ri->m_PositionRatio.x * heightRatio, ri->m_PositionRatio.y * heightRatio };
				XMFLOAT2 newSize = { ri->m_SizeRatio.x, ri->m_SizeRatio.y * Core::g_DisplayHeight };

				newSize.x = newSize.x * newSize.y;

				XMFLOAT2 newPos = { 0.f, 0.f };

				newPos.x = (offset.x + newSize.x * 0.05f);
				newPos.y = -(offset.y + newSize.y * 0.05f);

				ri->m_World._11 = newSize.x;
				ri->m_World._22 = newSize.y;

				ri->m_World._41 = newPos.x;
				ri->m_World._42 = newPos.y;

				if (ri->m_IsText)
				{
					ri->m_UIPos = newPos;
					ri->m_UISize = newSize;
				}
			}
			break;
		case TextAlignType::Left:
			break;
		case TextAlignType::Right:
			break;
		case TextAlignType::Top:
			break;
		case TextAlignType::Bottom:
			break;
		case TextAlignType::LT:
			if (Core::g_DisplayWidth > Core::g_DisplayHeight)
			{
				if (Core::g_DisplayHeight > 480)
				{
					float heightRatio = Core::g_DisplayHeight / 1080.f;

					XMFLOAT2 offset = { ri->m_PositionRatio.x - heightRatio, ri->m_PositionRatio.y - heightRatio };
					XMFLOAT2 newSize = { ri->m_SizeRatio.x, ri->m_SizeRatio.y };

					XMFLOAT2 newPos = { 0.f, 0.f };

					newSize.y = newSize.y * heightRatio;
					newSize.x = newSize.y * newSize.x;

					newPos.x = (-(Core::g_DisplayWidth / 2.f) + (offset.x + newSize.x * 0.05f));
					newPos.y = (Core::g_DisplayHeight / 2.f) - (offset.y - newSize.y * 0.05f) * heightRatio;


					ri->m_World._11 = newSize.x;
					ri->m_World._22 = newSize.y;

					ri->m_World._41 = newPos.x;
					ri->m_World._42 = newPos.y;

					if (ri->m_IsText)
					{
						ri->m_UIPos = newPos;
						ri->m_UISize = newSize;
					}
				}
			}

			break;
		case TextAlignType::LB:
			break;
		case TextAlignType::RT:
			break;
		case TextAlignType::RB:
			break;
		default:
			break;
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

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&mMainPassCB.Ortho, XMMatrixTranspose(ortho));
	XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));

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

UITextInfo GraphicsContext::GetUIPosAndSize(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, std::string uiName)
{
	if (!objInfo) return UITextInfo{};

	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	for (auto& i : info)
	{
		UserInterface* ri = static_cast<UserInterface*>(rItems[i.second]);

		if (ri->m_IsText && ri->GetInstID() == uiName)
		{
			return UITextInfo{ ri->m_UIPos, ri->m_UISize };
		}
	}

	return UITextInfo{};
}

void GraphicsContext::UpdateSkinnedCBs(UINT skinnedCBIndex, SkinnedModelInstance* skinmodelInstance, int playerNum)
{
	//skinmodelInstance->UpdateSkinnedAnimation(0);

	ShaderResource::SkinnedConstants skinnedConstants;
	std::copy(
		std::begin(skinmodelInstance->FinalTransforms),
		std::end(skinmodelInstance->FinalTransforms),
		&skinnedConstants.BoneTransforms[0]);

	if (playerNum == 1)
	{
		//cout << test << " - " ;
		//cout << "x: " << skinmodelInstance->FinalTransforms[test]._31 << " , ";
		//cout << "y: " << skinmodelInstance->FinalTransforms[test]._32 << " , ";
		//cout << "z: " << skinmodelInstance->FinalTransforms[test]._33 << endl;
		//GameObject* leftWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(1));
		////leftWeapon->SetWorldMatrix(skinmodelInstance->FinalTransforms[test]);
		//leftWeapon->SetPosition(
		//	skinmodelInstance->FinalTransforms[test]._41,
		//	skinmodelInstance->FinalTransforms[test]._42,
		//	skinmodelInstance->FinalTransforms[test]._43);
		//test++;
		//test %= 39;
		//GameObject* rightWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(2));
		//rightWeapon->SetWorldMatrix(skinmodelInstance->FinalTransforms[2]);
	}
	//else
	//{
	//	GameObject* leftWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(3));
	//	leftWeapon->SetWorldMatrix(skinmodelInstance->FinalTransforms[9]);
	//	GameObject* rightWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(4));
	//	rightWeapon->SetWorldMatrix(skinmodelInstance->FinalTransforms[19]);
	//}

	m_SkinnedCBs[skinnedCBIndex]->CopyData(0, skinnedConstants);
}

void GraphicsContext::DrawRenderItem(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, bool isShadow, int zLayer)
{
	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();

	for (auto& i : info)
	{
		auto ri = rItems[i.second];

		if (ri->m_IsVisible && ri->m_ZLayer == zLayer)
		{
			if (isShadow)
			{
				if (rItems[i.second]->GetMeshName() == OBJECT_MESH_STR_WALL_21 ||
					rItems[i.second]->GetMeshName() == OBJECT_MESH_STR_WALL_33)
					continue;
			}
#ifdef FRUSTUM_CULLMODE
			if (rItems[i.second]->m_IsCulling)
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

			Core::g_CommandList->DrawIndexedInstanced(ri->m_IndexCount, info.size(), ri->m_StartIndexLocation, ri->m_BaseVertexLocation, 0);
			ri->m_IsVisible = ri->m_IsVisibleOnePassCheck;
		}
	}
}

void GraphicsContext::DrawBoundingBox(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, bool drawAABB, int zLayer)
{
	const std::map<std::string, UINT>& info = objInfo->GetInstanceKeyMap();
	// Draw AABB
	for (auto& i : info)
	{
		auto ri = rItems[i.second];
		if (ri->m_IsAABB == drawAABB)
		{
			if (ri->m_ZLayer == zLayer)
			{
#ifdef FRUSTUM_CULLMODE
				if (rItems[i.second]->m_IsCulling)
				{
					XMMATRIX world = XMLoadFloat4x4(&rItems[i.second]->m_World);
					XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
					if (!TOY_GROUND::GetApp()->m_Camera->IsInFrustum(invWorld, rItems[i.second]->m_Bounds)) continue;
				}
#endif
				Core::g_CommandList->IASetVertexBuffers(0, 1, &ri->m_Bb->VertexBufferView());
				Core::g_CommandList->IASetIndexBuffer(&ri->m_Bb->IndexBufferView());
				Core::g_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

				auto instanceBuffer = m_InstanceBuffers[ri->GetMeshName()]->Resource();
				Core::g_CommandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->GetGPUVirtualAddress());

				Core::g_CommandList->DrawIndexedInstanced(24, info.size(), 0, ri->m_BaseVertexLocationBb, 0);
			}
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

// Font
void GraphicsContext::LoadFont(const wstring fontName, float fontSize)
{
	m_FontName = fontName;
	m_FontSize = fontSize;

	// Create D2D/DWrite objects for rendering text.
	ThrowIfFailed(Core::g_D2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_TextBrush));
	ThrowIfFailed(Core::g_DWriteFactory->CreateTextFormat(
		fontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		&m_TextFormat
	));
	ThrowIfFailed(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
	ThrowIfFailed(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

void GraphicsContext::SetTextSize(float fontSize)
{
	if (m_FontName.empty()) return;
	if (fontSize < 1.f) return;
	if (m_FontSize == fontSize) return;

	m_TextFormat.Reset();
	m_FontSize = fontSize;
	// Create D2D/DWrite objects for rendering text.
	ThrowIfFailed(Core::g_D2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_TextBrush));
	ThrowIfFailed(Core::g_DWriteFactory->CreateTextFormat(
		m_FontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_FontSize,
		L"en-us",
		&m_TextFormat
	));
	ThrowIfFailed(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
	ThrowIfFailed(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

void GraphicsContext::SetTextSize(float fontSize, DWRITE_TEXT_ALIGNMENT textType, D2D1::ColorF color)
{
	if (m_FontName.empty()) return;
	if (fontSize < 1.f) return;
	if (m_FontSize == fontSize) return;

	m_TextFormat.Reset();
	m_FontSize = fontSize;
	// Create D2D/DWrite objects for rendering text.
	ThrowIfFailed(Core::g_D2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(color), &m_TextBrush));
	ThrowIfFailed(Core::g_DWriteFactory->CreateTextFormat(
		m_FontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_FontSize,
		L"en-us",
		&m_TextFormat
	));
	ThrowIfFailed(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
	ThrowIfFailed(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

void GraphicsContext::SetTextAlignMent(DWRITE_TEXT_ALIGNMENT textAlignment)
{
	ThrowIfFailed(m_TextFormat->SetTextAlignment(textAlignment));
}

void GraphicsContext::SetColor(D2D1::ColorF color)
{
	ThrowIfFailed(Core::g_D2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(color), &m_TextBrush));
}

void GraphicsContext::DrawD2DText(const wstring wtext, float posX, float posY, float rectSize)
{
	D2D1_RECT_F textRect;
	textRect.left = posX - rectSize + Core::g_DisplayWidth / 2;
	textRect.top = posY - rectSize + Core::g_DisplayHeight / 2;
	textRect.right = posX + rectSize + Core::g_DisplayWidth / 2;
	textRect.bottom = posY + rectSize + Core::g_DisplayHeight / 2;


	Core::g_D2dDeviceContext->DrawText(
		wtext.c_str(),
		wtext.size(),
		m_TextFormat.Get(),
		&textRect,
		m_TextBrush.Get()
	);
}

void GraphicsContext::DrawD2DText(const wstring wtext, float posX, float posY, float rectSizeX, float rectSizeY, bool aligmentLeading)
{
	if (!aligmentLeading)
		return;

	// 좌상단이 (0, 0)으로 설정
	D2D1_RECT_F textRect;
	textRect.left = posX - rectSizeX + Core::g_DisplayWidth;
	textRect.top = posY - rectSizeY;
	textRect.right = posX + rectSizeX;
	textRect.bottom = posY + rectSizeY;


	Core::g_D2dDeviceContext->DrawText(
		wtext.c_str(),
		wtext.size(),
		m_TextFormat.Get(),
		&textRect,
		m_TextBrush.Get()
	);
}

void GraphicsContext::UpdateShadowPassCB()
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	UINT w = GraphicsRenderer::GetApp()->mShadowMap->Width();
	UINT h = GraphicsRenderer::GetApp()->mShadowMap->Height();

	XMStoreFloat4x4(&mShadowPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mShadowPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mShadowPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mShadowPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mShadowPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mShadowPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mShadowPassCB.EyePosW = mLightPosW;
	mShadowPassCB.RenderTargetSize = XMFLOAT2((float)w, (float)h);
	mShadowPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / w, 1.0f / h);
	mShadowPassCB.NearZ = mLightNearZ;
	mShadowPassCB.FarZ = mLightFarZ;

	auto currPassCB = PassCB.get();
	currPassCB->CopyData(1, mShadowPassCB);
}

void GraphicsContext::UpdateShadowTransform(Light* light, DirectX::BoundingSphere sceneBounds)
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = DirectX::XMLoadFloat3(&light->Direction);
	XMVECTOR lightPos = -2.0f * sceneBounds.Radius * lightDir;
	XMVECTOR targetPos = DirectX::XMLoadFloat3(&sceneBounds.Center);
	XMVECTOR lightUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = DirectX::XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&mLightPosW, lightPos);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - sceneBounds.Radius;
	float b = sphereCenterLS.y - sceneBounds.Radius;
	float n = sphereCenterLS.z - sceneBounds.Radius;
	float r = sphereCenterLS.x + sceneBounds.Radius;
	float t = sphereCenterLS.y + sceneBounds.Radius;
	float f = sphereCenterLS.z + sceneBounds.Radius;

	mLightNearZ = n;
	mLightFarZ = f;
	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj * T;
	DirectX::XMStoreFloat4x4(&mLightView, lightView);
	XMStoreFloat4x4(&mLightProj, lightProj);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void GraphicsContext::SetResourceShadowPassCB()
{
	Core::g_CommandList->RSSetViewports(1, &GraphicsRenderer::GetApp()->mShadowMap->Viewport());
	Core::g_CommandList->RSSetScissorRects(1, &GraphicsRenderer::GetApp()->mShadowMap->ScissorRect());

	// Change to DEPTH_WRITE.
	Core::g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GraphicsRenderer::GetApp()->mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ShaderResource::PassConstants));

	// Clear the back buffer and depth buffer.
	Core::g_CommandList->ClearDepthStencilView(GraphicsRenderer::GetApp()->mShadowMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	Core::g_CommandList->OMSetRenderTargets(0, nullptr, false, &GraphicsRenderer::GetApp()->mShadowMap->Dsv());

	// Bind the pass constant buffer for the shadow map pass.
	auto passCB = GraphicsContext::GetApp()->PassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
	Core::g_CommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);
}

void GraphicsContext::ShadowTransitionResourceBarrier()
{
	// Change back to GENERIC_READ so we can read the texture in a shader.
	Core::g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GraphicsRenderer::GetApp()->mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void GraphicsContext::OnBlurEffect(bool onoff)
{
	GraphicsRenderer::GetApp()->m_SwitchBlur = onoff;
}

void GraphicsContext::OnResizeBlur()
{
	GraphicsRenderer::GetApp()->ExecuteResizeBlur();
}
