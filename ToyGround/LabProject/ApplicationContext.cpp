#include "pch.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "Map.h"
#include "Character.h"
#include "CharacterParts.h"

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

XMFLOAT3 ApplicationContext::FindSpawnLocation(std::string mapName, XMFLOAT4 spawnpos)
{
	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec) {
		XMFLOAT3 t{ spawnpos.x, spawnpos.y, spawnpos.z };
		if (XMVector3Equal(XMLoadFloat3(&t), XMLoadFloat3(&p.spawnPos)))
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

void ApplicationContext::CreateCharacter(std::string meshName, std::string instID, std::string matName, int skinnedCBIndex)
{
	Character* chr = CreateObject<Character>(meshName, instID);
	chr->SetMesh(meshName, meshName);
	chr->SetMaterial(AssertsReference::GetApp()->m_Materials[matName]->DiffuseSrvHeapIndex);
	chr->SetAnimationController(AssertsReference::GetApp()->m_SkinnedModelInsts[meshName].get());
	chr->m_SkinnedCBIndex = skinnedCBIndex;
	chr->m_PlayerID = skinnedCBIndex;
}

void ApplicationContext::CreateWeapon(std::string weaponName, std::string subWeaponName, std::string partName)
{
	CharacterParts* weapon = CreateObject<CharacterParts>(weaponName, subWeaponName);
	weapon->SetMesh(weaponName, subWeaponName);
	weapon->m_MaterialIndex = TEXTURE_INDEX_PolygonMinis_Texture_01_A;
	weapon->m_PartsName = partName;
	weapon->Rotate(0, 0, 0);
	weapon->SetPosition(0, 0, 0);
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

void ApplicationContext::DisplayCharacter(std::string mapName, Character* user, XMFLOAT4 spawnpos, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	if (!user) return;

	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec) {
		XMFLOAT3 t{ spawnpos.x, spawnpos.y, spawnpos.z };
		if (XMVector3Equal(XMLoadFloat3(&t), XMLoadFloat3(&p.spawnPos)))
		{
			user->InitializeTransform();
			user->SetAnimationKeyState(AnimationController::PlayerState::STATE_IDLE);
			user->SetAnimationPlayerState(AnimationController::PlayerState::STATE_IDLE);

			user->m_HP = 1.f;
			user->Rotate(0, XMConvertToRadians(p.rotY), 0);
			user->SetPosition(p.position);
		}
	}
}

void ApplicationContext::DisplayCharacter(std::string mapName, std::string userName, XMFLOAT4 spawnpos, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	Character* user = FindObject<Character>(userName, userName);
	if (!user) return;

	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec){
		XMFLOAT3 t{ spawnpos.x, spawnpos.y, spawnpos.z };
		if (XMVector3Equal(XMLoadFloat3(&t), XMLoadFloat3(&p.spawnPos)))
		{
			user->InitializeTransform();
			user->SetAnimationKeyState(AnimationController::PlayerState::STATE_IDLE);
			user->SetAnimationPlayerState(AnimationController::PlayerState::STATE_IDLE);

			user->Rotate(0, XMConvertToRadians(p.rotY), 0);
			user->SetPosition(p.position);

			return;
		}
	}
}
