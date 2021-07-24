#include "pch.h"
#include "Picking.h"
#include "ApplicationContext.h"
#include "TOY_GROUND.h"

#include "Map.h"
#include "GeometryMesh.h"

PickedObject Picking::RayIntersect2D(int screenX, int screenY)
{
	const std::map<std::string, UINT>& info = AppContext->m_RItemsMap[OBJECT_TYPE_UI2D]->GetInstanceKeyMap();

	if (info.size() < 1) return {};

	PickedObject pickedObject;

	XMFLOAT3 scale{};
	XMFLOAT3 posLB{};
	XMFLOAT3 posRT{};


	for (auto& p : info)
	{
		auto ri = AppContext->m_RItemsVec[p.second];

		if (ri->m_IsVisible)
		{
			scale = { ri->m_World._11 , ri->m_World._22, ri->m_World._33 };
			posLB = { ri->m_World._41 - ((scale.x / 20.f)), ri->m_World._42 + ((scale.y / 20.f)), ri->m_World._43 };
			posRT = { ri->m_World._41 + ((scale.x / 20.f)), ri->m_World._42 - ((scale.y / 20.f)) , ri->m_World._43 };

			posLB = { Core::g_DisplayWidth / 2.f + posLB.x,  -posLB.y + Core::g_DisplayHeight / 2.f, posLB.z };
			posRT = { Core::g_DisplayWidth / 2.f + posRT.x,  -posRT.y + Core::g_DisplayHeight / 2.f, posRT.z };

			if (screenX < posLB.x)
				continue;
			if (screenX > posRT.x)
				continue;
			if (screenY > posRT.y)
				continue;
			if (screenY < posLB.y)
				continue;

			pickedObject.distance = 0.0;
			pickedObject.instanceID = ri->GetIndex();
			pickedObject.objectName = ri->GetMeshName();
			pickedObject.instName = ri->GetInstID();
			pickedObject.materialIndex = ri->m_MaterialIndex;
			pickedObject.instName = ri->GetInstID();

			return pickedObject;
		}
	}

	return {};
}

PickedObject Picking::RayIntersect2D(int screenX, int screenY, std::string sceneName, std::string layer)
{
	const std::map<std::string, UINT>& info = AppContext->m_RItemsMap[layer + sceneName]->GetInstanceKeyMap();

	if (info.size() < 1) return {};

	PickedObject pickedObject;

	XMFLOAT3 scale{};
	XMFLOAT3 posLB{};
	XMFLOAT3 posRT{};


	for (auto& p : info)
	{
		auto ri = AppContext->m_RItemsVec[p.second];

		if (ri->m_IsVisible)
		{
			scale = { ri->m_World._11 , ri->m_World._22, ri->m_World._33 };
			posLB = { ri->m_World._41 - ((scale.x / 20.f)), ri->m_World._42 + ((scale.y / 20.f)), ri->m_World._43 };
			posRT = { ri->m_World._41 + ((scale.x / 20.f)), ri->m_World._42 - ((scale.y / 20.f)) , ri->m_World._43 };

			posLB = { Core::g_DisplayWidth / 2.f + posLB.x,  -posLB.y + Core::g_DisplayHeight / 2.f, posLB.z };
			posRT = { Core::g_DisplayWidth / 2.f + posRT.x,  -posRT.y + Core::g_DisplayHeight / 2.f, posRT.z };
			// posLB = { Core::g_DisplayWidth / 2.f + posLB.x * Core::g_DisplayWidth / 2.f,  -posLB.y * Core::g_DisplayHeight / 2.f + Core::g_DisplayHeight / 2.f, posLB.z };
			// posRT = { Core::g_DisplayWidth / 2.f + posRT.x * Core::g_DisplayWidth / 2.f,  -posRT.y * Core::g_DisplayHeight / 2.f + Core::g_DisplayHeight / 2.f, posRT.z };

			if (screenX < posLB.x)
				continue;
			if (screenX > posRT.x)
				continue;
			if (screenY > posRT.y)
				continue;
			if (screenY < posLB.y)
				continue;


			pickedObject.distance = 0.0;
			pickedObject.instanceID = ri->GetIndex();
			pickedObject.objectName = ri->GetMeshName();
			pickedObject.instName = ri->GetInstID();
			pickedObject.materialIndex = ri->m_MaterialIndex;

			return pickedObject;
		}
	}

	return {};
}

PickedObject Picking::RayIntersect2DZLayer(int screenX, int screenY, std::string sceneName, std::string layer)
{
	const std::map<std::string, UINT>& info = AppContext->m_RItemsMap[layer + sceneName]->GetInstanceKeyMap();
	
	if (info.size() < 1) return {};
	
	PickedObject pickedObject;
	
	
	for (auto& p : info)
	{
		auto ri = AppContext->m_RItemsVec[p.second];
		if (ri->m_ZLayer == UI_LAYER_0)
		{
			pickedObject = RayIntersect2D(screenX, screenY, sceneName, layer);
		}
	}
	
	if (pickedObject.objectName != "")
		return pickedObject;
	
	for (auto& p : info)
	{
		auto ri = AppContext->m_RItemsVec[p.second];
		if (ri->m_ZLayer == UI_LAYER_1)
		{
			pickedObject = RayIntersect2D(screenX, screenY, sceneName, layer);
		}
	}
	
	if (pickedObject.objectName != "")
		return pickedObject;
	
	for (auto& p : info)
	{
		auto ri = AppContext->m_RItemsVec[p.second];
		if (ri->m_ZLayer == -1)
		{
			pickedObject = RayIntersect2D(screenX, screenY, sceneName, layer);
		}
	}
	
	if (pickedObject.objectName != "")
		return pickedObject;
	
	return {};
}

void Picking::RayAtViewSpace(XMVECTOR& original, XMVECTOR& direction, int screenX, int screenY)
{
	if (!TOY_GROUND::GetApp()->m_Camera)
		return;

	XMFLOAT4X4 matProjection = TOY_GROUND::GetApp()->m_Camera->GetProj4x4f();

	float vx = (+2.0f * screenX / Core::g_DisplayWidth - 1.0f) / matProjection._11;
	float vy = (-2.0f * screenY / Core::g_DisplayHeight + 1.0f) / matProjection._22;

	original = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	direction = XMVectorSet(vx, vy, 1.0f, 0.0f);
}


