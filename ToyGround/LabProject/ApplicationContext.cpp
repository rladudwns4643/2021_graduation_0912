#include "pch.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "Map.h"
#include "Character.h"

#define WIDTH_NORMALIZE_LT(x) (x + (1980.f / 2.f))
#define HEIGHT_NORMALIZE_LT(y) (-y + (1080.f / 2.f))


std::string ApplicationContext::FindMapName(int mapCode) const
{
	std::string mapName;

	switch (mapCode)
	{
	case GAME_MAP:
		mapName = MAP_STR_GAME_MAP;
		break;
	case TEMP_SECOND_MAP:
		mapName = MAP_STR_SECONDMAP;
		break;
	}

	return mapName;
}

XMFLOAT3 ApplicationContext::FindSpawnLocation(std::string mapName, int spawnLocation)
{
	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec)
	{
		if (spawnLocation == p.spawnPos)
		{
			return p.position;
		}
	}

	return XMFLOAT3(0, 0, 0);
}

void ApplicationContext::CreateSkycube(std::string skycubeName, std::string instID, std::string matName)
{
	/* SkyCube */
	GameObject* sky = CreateObject<GameObject>(skycubeName, instID);
	sky->SetMesh(MESH_GEOID, MESH_GEOID_SPHERE);
	sky->m_MaterialIndex = AssertsReference::GetApp()->m_Materials[matName]->DiffuseSrvHeapIndex;
	sky->m_World = MathHelper::Identity4x4();
	sky->m_TexTransform = MathHelper::Identity4x4();

	sky->Scale(SKYBOX_SCALE, SKYBOX_SCALE, SKYBOX_SCALE);
}

void ApplicationContext::CreateProps(std::string mapName)
{
	std::vector<MapTool::MapInfo> mapInfoVector = m_Maps[mapName]->mapInfoVector;

	for (auto& itemInfo : mapInfoVector)
	{
		GameObject* item = CreateObject<GameObject>(itemInfo.meshName, std::to_string(itemInfo.typeID));
		item->SetMesh(itemInfo.meshName, itemInfo.meshName);
		item->m_MaterialIndex = AssertsReference::GetApp()->m_Materials[itemInfo.textureName]->DiffuseSrvHeapIndex;
	}
}

void ApplicationContext::CreateCharacter(std::string meshName, std::string instID, std::string matName, int spawnLocation)
{
	Character* chr = CreateObject<Character>(meshName, instID);
	chr->SetMesh(MESH_GEOID, MESH_GEOID_SPHERE);
	chr->SetMaterial(AssertsReference::GetApp()->m_Materials[matName]->DiffuseSrvHeapIndex);
	chr->m_SpawnLoaction = spawnLocation;
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
		if (isScale) obj->Scale(scaleValue, scaleValue, scaleValue);
		obj->Rotate(itemInfo.rotation.x, itemInfo.rotation.y, itemInfo.rotation.z);
		obj->SetPosition(itemInfo.position);
	}
}

void ApplicationContext::DisplayCharacter(std::string mapName, Character* user, int spawnLocation, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	if (!user) return;

	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec)
	{
		if (spawnLocation == p.spawnPos)
		{
			user->InitializeTransform();
			user->m_HP = 1.f;
			user->Rotate(0, XMConvertToRadians(p.rotY), 0);
			user->SetPosition(p.position);
		}
	}
}