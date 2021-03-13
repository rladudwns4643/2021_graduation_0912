#include "pch.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "Character.h"

#define WIDTH_NORMALIZE_LT(x) (x + (1980.f / 2.f))
#define HEIGHT_NORMALIZE_LT(y) (-y + (1080.f / 2.f))

void ApplicationContext::CreateProps(std::string mapName)
{
	for (auto& itemInfo : mapInfoVector)
	{
		GameObject* item = CreateObject<GameObject>(itemInfo.meshName, std::to_string(itemInfo.typeID));
		item->SetServerMeshID(itemInfo.proptype);
		item->SetMesh(itemInfo.meshName, itemInfo.meshName);
		item->m_MaterialIndex = AssertsReference::GetApp()->m_Materials[itemInfo.textureName]->DiffuseSrvHeapIndex;
		item->m_IsVisible = false;
		item->m_IsVisibleOnePassCheck = false;
		item->m_IsChangeable = itemInfo.changeable;
	}
}

void ApplicationContext::CreateContourProps(std::string mapName)
{
	for (auto& itemInfo : mapInfoVector)
	{
		GameObject* item = CreateObject<GameObject>(itemInfo.meshName + "Contour", std::to_string(itemInfo.typeID));
		item->SetServerMeshID(itemInfo.proptype);
		item->SetMesh(itemInfo.meshName, itemInfo.meshName);
		item->m_MaterialIndex = AssertsReference::GetApp()->m_Materials[itemInfo.textureName]->DiffuseSrvHeapIndex;
		item->m_IsVisible = false;
		item->m_IsChangeable = itemInfo.changeable;
		item->m_World = FindObject<GameObject>(itemInfo.meshName, to_string(itemInfo.typeID))->m_World;
	}

}

void ApplicationContext::CreateCharacter(std::string meshName, std::string instID, std::string matName, int skinnedCBIndex)
{
	Character* chr = CreateObject<Character>(meshName, instID);
	chr->SetMesh(meshName, meshName);
	chr->SetMaterial(AssertsReference::GetApp()->m_Materials[matName]->DiffuseSrvHeapIndex);
	chr->m_IsVisible = false;
	chr->m_IsVisibleOnePassCheck = false;
	chr->m_IsChangeable = false;
	// 임시 스폰위치 지정
	chr->m_SpawnLoaction = skinnedCBIndex;
}

void ApplicationContext::DisplayProps(std::string mapName, bool isScale, float scaleValue)
{
	if (!m_Maps.count(mapName))
		return;

	// Props
	std::vector<MapTool::MapInfo> mapInfoVector = m_Maps[mapName]->mapInfoVector;
	for (auto& itemInfo : mapInfoVector)
	{
		GameObject* obj = FindObject<GameObject>(itemInfo.meshName, std::to_string(itemInfo.typeID));

		obj->InitializeTransform();
		obj->m_IsVisible = true;
		obj->m_IsVisibleOnePassCheck = true;
		if (isScale) obj->Scale(scaleValue, scaleValue, scaleValue);
		obj->Rotate(itemInfo.rotation.x, itemInfo.rotation.y, itemInfo.rotation.z);
		obj->SetPosition(itemInfo.position);
	}
}

void ApplicationContext::DisplayCharacter(std::string mapName, Character* user, int spawnLocation, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	if (!user) return;

	for (auto& p : playerInfoVec)
	{
		if (spawnLocation == p.spawnPos)
		{
			user->InitializeTransform();
			user->m_IsVisible = isVisible;
			user->m_IsVisibleOnePassCheck = isVisible;
			user->m_HP = 1.f;
			user->Rotate(0, XMConvertToRadians(p.rotY), 0);
			user->SetPosition(p.position);
		}
	}
}

void ApplicationContext::DisplayCharacter(std::string mapName, std::string userName, int spawnLocation, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	Character* user = FindObject<Character>(userName, userName);
	if (!user) return;

	for (auto& p : playerInfoVec)
	{
		if (spawnLocation == p.spawnPos)
		{
			user->InitializeTransform();
			
			user->m_IsVisible = isVisible;
			user->m_IsVisibleOnePassCheck = isVisible;
			user->Rotate(0, XMConvertToRadians(p.rotY), 0);
			user->SetPosition(p.position);

			return;
		}
	}
}

void ApplicationContext::CreateDebugBoundingBox(std::string boundsName, std::string boundsInstName)
{
	GameObject* item = CreateObject<GameObject>(boundsName, boundsInstName);
	item->SetMesh(MESH_GEOID, MESH_GEOID_RECT);
	item->m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->m_MaterialIndex = 1;
	item->m_IsVisible = true;
	item->m_IsChangeable = true;
}
