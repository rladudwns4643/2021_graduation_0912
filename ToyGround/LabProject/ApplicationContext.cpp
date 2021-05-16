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

XMFLOAT3 ApplicationContext::FindSpawnLocation(std::string mapName, Character* user)
{
	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec) {
		if (user->m_UserPlayerMeshName == p.playerName)
		{
			return p.spawnPos;
		}
	}

	return XMFLOAT3(0, 0, 0);
}

XMFLOAT3 ApplicationContext::FindSpawnLocation(std::string mapName, std::string userName)
{
	Character* user = FindObject<Character>(userName, userName);
	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec) {
		if (userName == p.playerName)
		{
			return p.spawnPos;
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
		item->SetBBMesh(itemInfo.meshName);
		item->m_MaterialIndex = AssertsReference::GetApp()->m_Materials[itemInfo.textureName]->DiffuseSrvHeapIndex;
		item->m_Bounds.Center = AssertsReference::GetApp()->m_PropBoundingBox[itemInfo.meshName]->Center;
		item->m_Bounds.Extents = AssertsReference::GetApp()->m_PropBoundingBox[itemInfo.meshName]->Extents;
		item->m_IsAABB = true;
		if (itemInfo.meshName == OBJECT_MESH_STR_ATTACK_BOX)
			item->m_IsAABB = false;
	}
}

void ApplicationContext::CreateCharacter(std::string meshName, std::string instID, std::string matName, int skinnedCBIndex)
{
	Character* chr = CreateObject<Character>(meshName, instID);
	chr->SetMesh(meshName, meshName);
	chr->SetBBMesh(meshName);
	chr->SetMaterial(AssertsReference::GetApp()->m_Materials[matName]->DiffuseSrvHeapIndex);
	chr->SetAnimationController(AssertsReference::GetApp()->m_SkinnedModelInsts[meshName].get());
	chr->m_UserPlayerMeshName = meshName;
	chr->m_SkinnedCBIndex = skinnedCBIndex;
	chr->m_PlayerID = skinnedCBIndex;
	chr->m_Bounds.Center = AssertsReference::GetApp()->m_PropBoundingBox[meshName]->Center;
	chr->m_Bounds.Extents = AssertsReference::GetApp()->m_PropBoundingBox[meshName]->Extents;
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

void ApplicationContext::CreateGem()
{
	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		GameObject* gem = CreateObject<GameObject>(OBJECT_MESH_STR_GEM, std::to_string(OBJECT_START_INDEX_GEM + i));
		gem->SetMesh(OBJECT_MESH_STR_GEM, OBJECT_MESH_STR_GEM);
		gem->SetBBMesh(OBJECT_MESH_STR_GEM);
		gem->m_MaterialIndex = TEXTURE_INDEX_Cartoon_CubeWorld_Texture;
		gem->m_Bounds.Center = AssertsReference::GetApp()->m_PropBoundingBox[OBJECT_MESH_STR_GEM]->Center;
		gem->m_Bounds.Extents = AssertsReference::GetApp()->m_PropBoundingBox[OBJECT_MESH_STR_GEM]->Extents;
		gem->m_IsAABB = true;
		gem->m_IsVisible = false;
	}
}
void ApplicationContext::DisplayGem(int instID, float posX, float posY, float posZ)
{
	GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_GEM, std::to_string(OBJECT_START_INDEX_GEM + instID));
	if (!obj) return;

	obj->m_IsVisible = true;
	obj->InitializeTransform();
	obj->Scale(1, 1, 1);
	obj->SetPosition(posX, posY, posZ);
}

void ApplicationContext::HiddenGem(int instID, bool isVisible)
{
	GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_GEM, std::to_string(OBJECT_START_INDEX_GEM + instID));
	if (!obj) {
		cout << "HiddenGem: cant find obj" << endl;
		return;
	}

	ZeroMemory(&obj->m_World, sizeof(obj->m_World));
	ZeroMemory(&obj->m_TexTransform, sizeof(obj->m_TexTransform));

	if (isVisible)
	{
		GraphicsContext::GetApp()->UpdateInstanceData(m_RItemsMap[OBJECT_MESH_STR_GEM], m_RItemsVec);
	}
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
		if(obj->GetMeshName() == OBJECT_MESH_STR_WALL_21)
			obj->Scale(1.0f, 5.5, 21.f);
		else if (obj->GetMeshName() == OBJECT_MESH_STR_WALL_33)
			obj->Scale(1.0f, 5.5, 33.f);
		obj->Rotate(itemInfo.rotation.x, itemInfo.rotation.y, itemInfo.rotation.z);
		obj->SetPosition(itemInfo.position);
	}
}

void ApplicationContext::DisplayCharacter(std::string mapName, Character* user, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	if (!user) return;

	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec) {
		if (user->m_UserPlayerMeshName == p.playerName)
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

void ApplicationContext::DisplayCharacter(std::string mapName, std::string userName, bool isVisible)
{
	if (!m_Maps.count(mapName)) return;
	Character* user = FindObject<Character>(userName, userName);
	if (!user) return;

	std::vector<MapTool::PlayerInfo> playerInfoVec = m_Maps[mapName]->playerInfoVector;

	for (auto& p : playerInfoVec){
		if(userName == p.playerName)
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
