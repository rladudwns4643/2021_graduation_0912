#include "pch.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "Map.h"
#include "Character.h"
#include "CharacterParts.h"
#include "UserInterface.h"
#include "Button.h"
#include "ImageView.h"
#include "Particle.h"

#define WIDTH_NORMALIZE_LT(x) (x + (1280.f / 2.f))
#define HEIGHT_NORMALIZE_LT(y) (-y + (720.f / 2.f))


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
	sky->m_IsVisible = true;
	sky->m_IsVisibleOnePassCheck = true;

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
		item->m_IsVisible = false;
		item->m_IsVisibleOnePassCheck = false;
		item->m_IsCulling = itemInfo.isCulling;
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
	chr->m_IsVisible = false;
	chr->m_IsVisibleOnePassCheck = false;
}

void ApplicationContext::CreateUI2D(std::string ui2dLayer, std::string ui2dName, int matIndex, int uiPressedTextureIdx, int uiDisabledTextureIdx)
{
	UserInterface* item = CreateObject<UserInterface>(ui2dLayer, ui2dName);
	item->SetMesh(MESH_GEOID, MESH_GEOID_GRID);
	item->m_MaterialIndex = matIndex;
	item->SetOriginMaterial(matIndex);
	item->m_IsVisible = false;
	item->m_IsVisibleOnePassCheck = false;
	item->m_IsCulling = false;

	item->SetReleasedTexture(matIndex);

	if (uiPressedTextureIdx != -1)
		item->SetPressedTexture(uiPressedTextureIdx);
	if (uiDisabledTextureIdx != -1)
		item->SetDisabledTexture(uiDisabledTextureIdx);
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
		gem->m_IsVisibleOnePassCheck = false;
		gem->m_IsCulling = true;
	}
}

void ApplicationContext::DisplayGem(int instID, float posX, float posY, float posZ)
{
	GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_GEM, std::to_string(OBJECT_START_INDEX_GEM + instID));
	if (!obj) return;

	obj->m_IsVisible = true;
	obj->m_IsVisibleOnePassCheck = true;
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
	obj->m_IsVisible = isVisible;
	obj->m_IsVisibleOnePassCheck = isVisible;

	if (isVisible)
	{
		GraphicsContext::GetApp()->UpdateInstanceData(m_RItemsMap[OBJECT_MESH_STR_GEM], m_RItemsVec);
	}
}

void ApplicationContext::CreateBullet()
{
	for (int i = 0; i < MAX_BULLET_COUNT; ++i)
	{
		GameObject* Bullet = CreateObject<GameObject>(OBJECT_MESH_STR_BULLET_01, std::to_string(OBJECT_START_INDEX_BULLET_01 + i));
		Bullet->SetMesh(OBJECT_MESH_STR_BULLET_01, OBJECT_MESH_STR_BULLET_01);
		Bullet->SetBBMesh(OBJECT_MESH_STR_BULLET_01);
		Bullet->m_MaterialIndex = TEXTURE_INDEX_Polygon_Plane_Texture_02;
		Bullet->m_Bounds.Center = AssertsReference::GetApp()->m_PropBoundingBox[OBJECT_MESH_STR_BULLET_01]->Center;
		Bullet->m_Bounds.Extents = AssertsReference::GetApp()->m_PropBoundingBox[OBJECT_MESH_STR_BULLET_01]->Extents;
		Bullet->m_IsAABB = true;
		Bullet->m_IsVisible = false;
		Bullet->m_IsVisibleOnePassCheck = false;
		Bullet->m_IsCulling = true;
		Bullet->InitializeTransform();
		Bullet->Scale(1, 1, 1);
		//cout << "C - Bullet: " << OBJECT_START_INDEX_BULLET_01 + i << endl;
	}
	for (int i = 0; i < MAX_SKILL_BULLET_COUNT; ++i)
	{
		GameObject* Bullet = CreateObject<GameObject>(OBJECT_MESH_STR_BULLET_02, std::to_string(OBJECT_START_INDEX_BULLET_02 + i));
		Bullet->SetMesh(OBJECT_MESH_STR_BULLET_02, OBJECT_MESH_STR_BULLET_02);
		Bullet->SetBBMesh(OBJECT_MESH_STR_BULLET_02);
		Bullet->m_MaterialIndex = TEXTURE_INDEX_Polygon_Plane_Texture_03;
		Bullet->m_Bounds.Center = AssertsReference::GetApp()->m_PropBoundingBox[OBJECT_MESH_STR_BULLET_02]->Center;
		Bullet->m_Bounds.Extents = AssertsReference::GetApp()->m_PropBoundingBox[OBJECT_MESH_STR_BULLET_02]->Extents;
		Bullet->m_IsAABB = true;
		Bullet->m_IsVisible = false;
		Bullet->m_IsVisibleOnePassCheck = false;
		Bullet->m_IsCulling = true;
		Bullet->InitializeTransform();
		Bullet->Scale(1, 1, 1);
	}
}

void ApplicationContext::UpdateBullet()
{
	int hiddenBulletIndex[MAX_BULLET_COUNT];
	int hiddenBulletCount = 0;
	Character* player1 = FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
	Character* player2 = FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN);
	XMFLOAT3 player1IndexPos = player1->GetIndexPos();
	XMFLOAT3 player2IndexPos = player2->GetIndexPos();
	XMFLOAT3 player1Pos = player1->GetPosition();
	XMFLOAT3 player2Pos = player2->GetPosition();
	BoundingBox player1Bound = player1->GetBoundingBox();
	BoundingBox player2Bound = player2->GetBoundingBox();
	
	for (int i = 0; i < m_ActiveBulletCnt; ++i)
	{
		GameObject* bullet = FindObject<GameObject>(OBJECT_MESH_STR_BULLET_01, std::to_string(OBJECT_START_INDEX_BULLET_01 + m_ActiveBullet[i]));
		if (bullet->m_IsVisible == false || !bullet) continue;
		bullet->Update();

		XMFLOAT3 pos = bullet->GetPosition();
		if (pos.x >= (MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE ||
			pos.x <= -((MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE) ||
			pos.z >= (MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE ||
			pos.z <= -((MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE) ||
			pos.y <= 0) {
			hiddenBulletIndex[hiddenBulletCount++] = m_ActiveBullet[i];
			continue;
		}

		XMFLOAT3 look = bullet->GetLook();
		bool endCheckX = false;
		bool endCheckY = false;
		bool endCheckZ = false;
		XMFLOAT3 endPos = bullet->m_EndPos;
		if (look.x >= 0 && pos.x >= endPos.x)
			endCheckX = true;
		if (look.x < 0 && pos.x <= endPos.x)
			endCheckX = true;
		if (look.y >= 0 && pos.y >= endPos.y)
			endCheckY = true;
		if (look.y < 0 && pos.y <= endPos.y)
			endCheckY = true;
		if (look.z >= 0 && pos.z >= endPos.z)
			endCheckZ = true;
		if (look.z < 0 && pos.z <= endPos.z)
			endCheckZ = true;
		if (endCheckX && endCheckY && endCheckZ)
		{
			hiddenBulletIndex[hiddenBulletCount++] = m_ActiveBullet[i];
			continue;
		}

		int indexPosX = (int)((pos.x + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_WIDTH_BLOCK_NUM / 2));
		int indexPosY = (int)(pos.y / STD_CUBE_SIZE) + 1;
		int indexPosZ = (int)((pos.z + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_DEPTH_BLOCK_NUM / 2));
		bool checkColl = false;
		Map* originMap = AppContext->m_Maps[MAP_STR_GAME_MAP];
		BoundingBox bound = bullet->GetBoundingBox();
		for (auto& p : originMap->mapInfoVector)
		{
			bool bulletCollObj = false;
			for (int j = 0; j < 81; j += 3)
			{
				int aX = indexPosX + shiftArr[j];
				int aY = indexPosY + shiftArr[j + 1];
				int aZ = indexPosZ + shiftArr[j + 2];

				if (aX < 0 || aX >= MAP_WIDTH_BLOCK_NUM
					|| aY <= 0 || aY >= MAP_HEIGHT_BLOCK_NUM
					|| aZ < 0 || aZ >= MAP_DEPTH_BLOCK_NUM)
					continue;

				// 캐릭터와 충돌
				if (bullet->m_FiredPlayerID == player2->m_PlayerID)
				{
					if (player1IndexPos.x == aX &&
						player1IndexPos.y == aY &&
						player1IndexPos.z == aZ)
					{
						XMFLOAT3 player1Min(player1Pos.x + player1Bound.Center.x - (player1Bound.Extents.x / 2),
							player1Pos.y + player1Bound.Center.y,
							player1Pos.z + player1Bound.Center.z - (player1Bound.Extents.z / 2));
						XMFLOAT3 bulletMin(pos.x + bound.Center.x - (bound.Extents.x / 2),
							pos.y + bound.Center.y,
							pos.z + bound.Center.z - (bound.Extents.z / 2));
						XMFLOAT3 player1Max(player1Pos.x + player1Bound.Center.x + (player1Bound.Extents.x / 2),
							player1Pos.y + player1Bound.Center.y + player1Bound.Extents.y,
							player1Pos.z + player1Bound.Center.z + (player1Bound.Extents.z / 2));
						XMFLOAT3 bulletMax(pos.x + bound.Center.x + (bound.Extents.x / 2),
							pos.y + bound.Center.y + bound.Extents.y,
							pos.z + bound.Center.z + (bound.Extents.z / 2));

						if (player1Min.x <= bulletMax.x && player1Max.x >= bulletMin.x &&
							player1Min.y <= bulletMax.y && player1Max.y >= bulletMin.y &&
							player1Min.z <= bulletMax.z && player1Max.z >= bulletMin.z)
						{
							hiddenBulletIndex[hiddenBulletCount++] = m_ActiveBullet[i];
							bulletCollObj = true;
							player1->m_hp -= 450;
							if (player2->m_skillGauge < MAX_SKILLGAUGE)
								player2->m_skillGauge += ONE_HIT_CHARGE_SKILLGAUGE;
							//cout << "Player1 HP: " << player1->m_hp << endl;
							break;
						}
					}
				}
				else if (bullet->m_FiredPlayerID == player1->m_PlayerID)
				{
					if (player2IndexPos.x == aX &&
						player2IndexPos.y == aY &&
						player2IndexPos.z == aZ)
					{
						XMFLOAT3 player2Min(player2Pos.x + player2Bound.Center.x - (player2Bound.Extents.x / 2),
							player2Pos.y + player2Bound.Center.y,
							player2Pos.z + player2Bound.Center.z - (player2Bound.Extents.z / 2));
						XMFLOAT3 bulletMin(pos.x + bound.Center.x - (bound.Extents.x / 2),
							pos.y + bound.Center.y,
							pos.z + bound.Center.z - (bound.Extents.z / 2));
						XMFLOAT3 player2Max(player2Pos.x + player2Bound.Center.x + (player2Bound.Extents.x / 2),
							player2Pos.y + player2Bound.Center.y + player2Bound.Extents.y,
							player2Pos.z + player2Bound.Center.z + (player2Bound.Extents.z / 2));
						XMFLOAT3 bulletMax(pos.x + bound.Center.x + (bound.Extents.x / 2),
							pos.y + bound.Center.y + bound.Extents.y,
							pos.z + bound.Center.z + (bound.Extents.z / 2));

						if (player2Min.x <= bulletMax.x && player2Max.x >= bulletMin.x &&
							player2Min.y <= bulletMax.y && player2Max.y >= bulletMin.y &&
							player2Min.z <= bulletMax.z && player2Max.z >= bulletMin.z)
						{
							hiddenBulletIndex[hiddenBulletCount++] = m_ActiveBullet[i];
							bulletCollObj = true;
							player2->m_hp -= 450;
							if (player1->m_skillGauge < MAX_SKILLGAUGE)
								player1->m_skillGauge += ONE_HIT_CHARGE_SKILLGAUGE;
							//cout << "Player2 HP: " << player2->m_hp << endl;
							break;
						}
					}
				}

				// 오브젝트와 충돌
				if (p.typeID == AppContext->m_MapArray[aY][aZ][aX] && p.colWithChar)
				{
					GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));

					XMFLOAT3 objPos = obj->GetPosition();
					XMFLOAT3 objMin(objPos.x + obj->m_Bounds.Center.x - (obj->m_Bounds.Extents.x / 2),
						objPos.y + obj->m_Bounds.Center.y,
						objPos.z + obj->m_Bounds.Center.z - (obj->m_Bounds.Extents.z / 2));
					XMFLOAT3 bulletMin(pos.x + bound.Center.x - (bound.Extents.x / 2),
						pos.y + bound.Center.y,
						pos.z + bound.Center.z - (bound.Extents.z / 2));
					XMFLOAT3 objMax(objPos.x + obj->m_Bounds.Center.x + (obj->m_Bounds.Extents.x / 2),
						objPos.y + obj->m_Bounds.Center.y + obj->m_Bounds.Extents.y,
						objPos.z + obj->m_Bounds.Center.z + (obj->m_Bounds.Extents.z / 2));
					XMFLOAT3 bulletMax(pos.x + bound.Center.x + (bound.Extents.x / 2),
						pos.y + bound.Center.y + bound.Extents.y,
						pos.z + bound.Center.z + (bound.Extents.z / 2));

					if (objMin.x <= bulletMax.x && objMax.x >= bulletMin.x &&
						objMin.y <= bulletMax.y && objMax.y >= bulletMin.y &&
						objMin.z <= bulletMax.z && objMax.z >= bulletMin.z)
					{
						hiddenBulletIndex[hiddenBulletCount++] = m_ActiveBullet[i];
						bulletCollObj = true;
						break;
					}
				}
			}
			if (bulletCollObj)
				break;
		}
	}
	for (int i = 0; i < hiddenBulletCount; ++i)
	{
		HiddenBullet(hiddenBulletIndex[i], 1);
	}

	int hiddenSkillBulletIndex[MAX_SKILL_BULLET_COUNT];
	int hiddenSkillBulletCount = 0;
	for (int i = 0; i < m_AtiveSkillBulletCnt; ++i)
	{
		GameObject* bullet = FindObject<GameObject>(OBJECT_MESH_STR_BULLET_02, std::to_string(OBJECT_START_INDEX_BULLET_02 + m_AtiveSkillBullet[i]));
		if (bullet->m_IsVisible == false || !bullet) continue;
		bullet->Update();

		XMFLOAT3 pos = bullet->GetPosition();
		if (pos.x >= (MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE ||
			pos.x <= -((MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE) ||
			pos.z >= (MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE ||
			pos.z <= -((MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE) ||
			pos.y <= 0)
		{
			hiddenSkillBulletIndex[hiddenSkillBulletCount++] = m_AtiveSkillBullet[i];
			continue;
		}

		XMFLOAT3 look = bullet->GetLook();
		bool endCheckX = false;
		bool endCheckY = false;
		bool endCheckZ = false;
		XMFLOAT3 endPos = bullet->m_EndPos;
		if (look.x >= 0 && pos.x >= endPos.x)
			endCheckX = true;
		if (look.x < 0 && pos.x <= endPos.x)
			endCheckX = true;
		if (look.y >= 0 && pos.y >= endPos.y)
			endCheckY = true;
		if (look.y < 0 && pos.y <= endPos.y)
			endCheckY = true;
		if (look.z >= 0 && pos.z >= endPos.z)
			endCheckZ = true;
		if (look.z < 0 && pos.z <= endPos.z)
			endCheckZ = true;
		if (endCheckX && endCheckY && endCheckZ)
		{
			hiddenSkillBulletIndex[hiddenSkillBulletCount++] = m_AtiveSkillBullet[i];
			continue;
		}

		int indexPosX = (int)((pos.x + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_WIDTH_BLOCK_NUM / 2));
		int indexPosY = (int)(pos.y / STD_CUBE_SIZE) + 1;
		int indexPosZ = (int)((pos.z + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_DEPTH_BLOCK_NUM / 2));
		bool checkColl = false;
		Map* originMap = AppContext->m_Maps[MAP_STR_GAME_MAP];
		BoundingBox bound = bullet->GetBoundingBox();
		for (auto& p : originMap->mapInfoVector)
		{
			bool bulletCollObj = false;
			for (int j = 0; j < 81; j += 3)
			{
				int aX = indexPosX + shiftArr[j];
				int aY = indexPosY + shiftArr[j + 1];
				int aZ = indexPosZ + shiftArr[j + 2];

				if (aX < 0 || aX >= MAP_WIDTH_BLOCK_NUM
					|| aY <= 0 || aY >= MAP_HEIGHT_BLOCK_NUM
					|| aZ < 0 || aZ >= MAP_DEPTH_BLOCK_NUM)
					continue;

				// 캐릭터와 충돌
				if (bullet->m_FiredPlayerID == player2->m_PlayerID)
				{
					if (player1IndexPos.x == aX &&
						player1IndexPos.y == aY &&
						player1IndexPos.z == aZ)
					{
						XMFLOAT3 player1Min(player1Pos.x + player1Bound.Center.x - (player1Bound.Extents.x / 2),
							player1Pos.y + player1Bound.Center.y,
							player1Pos.z + player1Bound.Center.z - (player1Bound.Extents.z / 2));
						XMFLOAT3 bulletMin(pos.x + bound.Center.x - (bound.Extents.x / 2),
							pos.y + bound.Center.y,
							pos.z + bound.Center.z - (bound.Extents.z / 2));
						XMFLOAT3 player1Max(player1Pos.x + player1Bound.Center.x + (player1Bound.Extents.x / 2),
							player1Pos.y + player1Bound.Center.y + player1Bound.Extents.y,
							player1Pos.z + player1Bound.Center.z + (player1Bound.Extents.z / 2));
						XMFLOAT3 bulletMax(pos.x + bound.Center.x + (bound.Extents.x / 2),
							pos.y + bound.Center.y + bound.Extents.y,
							pos.z + bound.Center.z + (bound.Extents.z / 2));

						if (player1Min.x <= bulletMax.x && player1Max.x >= bulletMin.x &&
							player1Min.y <= bulletMax.y && player1Max.y >= bulletMin.y &&
							player1Min.z <= bulletMax.z && player1Max.z >= bulletMin.z)
						{
							hiddenSkillBulletIndex[hiddenSkillBulletCount++] = m_AtiveSkillBullet[i];
							bulletCollObj = true;
							player1->m_hp -= 450;
							if (player2->m_skillGauge < MAX_SKILLGAUGE)
								player2->m_skillGauge += ONE_HIT_CHARGE_SKILLGAUGE;
							//cout << "Player1 HP: " << player1->m_hp << endl;
							break;
						}
					}
				}
				else if (bullet->m_FiredPlayerID == player1->m_PlayerID)
				{
					if (player2IndexPos.x == aX &&
						player2IndexPos.y == aY &&
						player2IndexPos.z == aZ)
					{
						XMFLOAT3 player2Min(player2Pos.x + player2Bound.Center.x - (player2Bound.Extents.x / 2),
							player2Pos.y + player2Bound.Center.y,
							player2Pos.z + player2Bound.Center.z - (player2Bound.Extents.z / 2));
						XMFLOAT3 bulletMin(pos.x + bound.Center.x - (bound.Extents.x / 2),
							pos.y + bound.Center.y,
							pos.z + bound.Center.z - (bound.Extents.z / 2));
						XMFLOAT3 player2Max(player2Pos.x + player2Bound.Center.x + (player2Bound.Extents.x / 2),
							player2Pos.y + player2Bound.Center.y + player2Bound.Extents.y,
							player2Pos.z + player2Bound.Center.z + (player2Bound.Extents.z / 2));
						XMFLOAT3 bulletMax(pos.x + bound.Center.x + (bound.Extents.x / 2),
							pos.y + bound.Center.y + bound.Extents.y,
							pos.z + bound.Center.z + (bound.Extents.z / 2));

						if (player2Min.x <= bulletMax.x && player2Max.x >= bulletMin.x &&
							player2Min.y <= bulletMax.y && player2Max.y >= bulletMin.y &&
							player2Min.z <= bulletMax.z && player2Max.z >= bulletMin.z)
						{
							hiddenSkillBulletIndex[hiddenSkillBulletCount++] = m_AtiveSkillBullet[i];
							bulletCollObj = true;
							player2->m_hp -= 450;
							if (player1->m_skillGauge < MAX_SKILLGAUGE)
								player1->m_skillGauge += ONE_HIT_CHARGE_SKILLGAUGE;
							//cout << "Player2 HP: " << player2->m_hp << endl;
							break;
						}
					}
				}


				if (p.typeID == AppContext->m_MapArray[aY][aZ][aX])
				{
					GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));

					XMFLOAT3 objPos = obj->GetPosition();
					XMFLOAT3 objMin(objPos.x + obj->m_Bounds.Center.x - (obj->m_Bounds.Extents.x / 2),
						objPos.y + obj->m_Bounds.Center.y,
						objPos.z + obj->m_Bounds.Center.z - (obj->m_Bounds.Extents.z / 2));
					XMFLOAT3 bulletMin(pos.x + bound.Center.x - (bound.Extents.x / 2),
						pos.y + bound.Center.y,
						pos.z + bound.Center.z - (bound.Extents.z / 2));
					XMFLOAT3 objMax(objPos.x + obj->m_Bounds.Center.x + (obj->m_Bounds.Extents.x / 2),
						objPos.y + obj->m_Bounds.Center.y + obj->m_Bounds.Extents.y,
						objPos.z + obj->m_Bounds.Center.z + (obj->m_Bounds.Extents.z / 2));
					XMFLOAT3 bulletMax(pos.x + bound.Center.x + (bound.Extents.x / 2),
						pos.y + bound.Center.y + bound.Extents.y,
						pos.z + bound.Center.z + (bound.Extents.z / 2));

					if (objMin.x <= bulletMax.x && objMax.x >= bulletMin.x &&
						objMin.y <= bulletMax.y && objMax.y >= bulletMin.y &&
						objMin.z <= bulletMax.z && objMax.z >= bulletMin.z)
					{
						if (p.canBroken)
						{
							ZeroMemory(&obj->m_World, sizeof(obj->m_World));
							ZeroMemory(&obj->m_TexTransform, sizeof(obj->m_TexTransform));
							obj->m_IsVisible = false;
							obj->m_IsVisibleOnePassCheck = false;
							AppContext->m_MapArray[aY][aZ][aX] = -1;
						}
						hiddenSkillBulletIndex[hiddenSkillBulletCount++] = m_AtiveSkillBullet[i];
						bulletCollObj = true;
						break;
					}
				}
			}
			if (bulletCollObj)
				break;
		}
	}
	for (int i = 0; i < hiddenSkillBulletCount; ++i)
	{
		HiddenBullet(hiddenSkillBulletIndex[i], 2);
	}
}

void ApplicationContext::DisplayBullet(int instID, XMFLOAT3 startPos, XMFLOAT3 look, int firedPlayerID, int bulletNum)
{
	if (bulletNum == 1)
	{
		GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_BULLET_01, std::to_string(OBJECT_START_INDEX_BULLET_01 + instID));
		if (!obj || obj->m_IsVisible) return;

		XMFLOAT3 bSpeed{ 0.f, 0.f, 0.f };
		bSpeed = MathHelper::Add(bSpeed, look, BULLET_SPEED);
		float t = 15;
		XMFLOAT3 temp{ startPos };
		temp.x += (bSpeed.x * t);
		temp.y += (bSpeed.y * t);
		temp.z += (bSpeed.z * t);
		XMFLOAT3 endPos{ temp };

		//cout << "speed: " << bSpeed.x << ", " << bSpeed.y << ", " << bSpeed.z << endl;
		//cout << "StartPos: " << startPos.x << ", " << startPos.y << ", " << startPos.z << endl;
		//cout << "EndPos: " << endPos.x << ", " << endPos.y << ", " << endPos.z << endl;

		obj->m_IsVisible = true;
		obj->m_IsVisibleOnePassCheck = true;
		obj->m_Speed = bSpeed;
		obj->SetMatrixByLook(look.x, look.y, look.z);
		obj->SetPosition(startPos);
		obj->m_EndPos = endPos;
		obj->m_FiredPlayerID = firedPlayerID;

		//cout << "D - Bullet: " << instID << endl;
	}
	else
	{
		GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_BULLET_02, std::to_string(OBJECT_START_INDEX_BULLET_02 + instID));
		if (!obj || obj->m_IsVisible) return;

		XMFLOAT3 bSpeed{ 0.f, 0.f, 0.f };
		bSpeed = MathHelper::Add(bSpeed, look, BULLET_SPEED);
		float t = 14;
		XMFLOAT3 temp{ startPos };
		temp.x += (bSpeed.x * t);
		temp.y += (bSpeed.y * t);
		temp.z += (bSpeed.z * t);
		XMFLOAT3 endPos{ temp };

		//cout << "speed: " << bSpeed.x << ", " << bSpeed.y << ", " << bSpeed.z << endl;
		//cout << "StartPos: " << startPos.x << ", " << startPos.y << ", " << startPos.z << endl;
		//cout << "EndPos: " << endPos.x << ", " << endPos.y << ", " << endPos.z << endl;

		obj->m_IsVisible = true;
		obj->m_IsVisibleOnePassCheck = true;
		obj->m_Speed = bSpeed;
		obj->SetMatrixByLook(look.x, look.y, look.z);
		obj->SetPosition(startPos);
		obj->m_EndPos = endPos;
		obj->m_FiredPlayerID = firedPlayerID;
	}
}

void ApplicationContext::HiddenBullet(int instID, int bulletNum)
{
	if (bulletNum == 1)
	{
		//cout << "H - Bullet: " << instID << endl;

		GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_BULLET_01, std::to_string(OBJECT_START_INDEX_BULLET_01 + instID));
		if (!obj) {
			cout << "HiddenBullet: cant find obj" << endl;
			return;
		}
		if (obj->m_IsVisible == false)
			return;
		obj->m_Speed = XMFLOAT3(0.f, 0.f, 0.f);
		obj->SetPosition(XMFLOAT3(0.f, -100.f, 0.f));

		obj->m_IsVisible = false;
		obj->m_IsVisibleOnePassCheck = false;
		m_ActiveBulletCheck[instID] = false;

		bool check = false;
		for (int j = 0; j < m_ActiveBulletCnt; ++j)
		{
			if (check)
				m_ActiveBullet[j - 1] = m_ActiveBullet[j];
			if (m_ActiveBullet[j] == instID)
				check = true;
		}
		m_ActiveBulletCnt--;
	}
	else
	{
		GameObject* obj = FindObject<GameObject>(OBJECT_MESH_STR_BULLET_02, std::to_string(OBJECT_START_INDEX_BULLET_02 + instID));
		if (!obj) {
			cout << "HiddenBullet: cant find obj" << endl;
			return;
		}
		if (obj->m_IsVisible == false)
			return;
		obj->m_Speed = XMFLOAT3(0.f, 0.f, 0.f);
		obj->SetPosition(XMFLOAT3(0.f, -100.f, 0.f));

		obj->m_IsVisible = false;
		obj->m_IsVisibleOnePassCheck = false;
		m_AtiveSkillBulletCheck[instID] = false;

		bool check = false;
		for (int j = 0; j < m_AtiveSkillBulletCnt; ++j)
		{
			if (check)
				m_AtiveSkillBullet[j - 1] = m_AtiveSkillBullet[j];
			if (m_AtiveSkillBullet[j] == instID)
				check = true;
		}
		m_AtiveSkillBulletCnt--;
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
		obj->m_IsVisible = true;
		obj->m_IsVisibleOnePassCheck = true;
		if (isScale) obj->Scale(scaleValue, scaleValue, scaleValue);
		if (obj->GetMeshName() == OBJECT_MESH_STR_WALL_21)
			obj->Scale(1.0f, 5.5f, 21.f);
		else if (obj->GetMeshName() == OBJECT_MESH_STR_WALL_33)
			obj->Scale(1.0f, 5.5f, 33.f);
		else if (obj->GetMeshName() == OBJECT_MESH_STR_FLOOR)
			obj->Scale(2.1f, 1.0f, 3.3f);
		obj->Rotate(itemInfo.rotation.x, itemInfo.rotation.y, itemInfo.rotation.z);
		obj->SetPosition(itemInfo.position);
	}
}

void ApplicationContext::HiddenProps(std::string mapName)
{
	if (!m_Maps.count(mapName))
		return;

	// Props
	std::vector<MapTool::MapInfo> mapInfoVector = m_Maps[mapName]->mapInfoVector;
	for (auto& itemInfo : mapInfoVector)
	{
		GameObject* obj = FindObject<GameObject>(itemInfo.meshName, std::to_string(itemInfo.typeID));

		ZeroMemory(&obj->m_World, sizeof(obj->m_World));
		ZeroMemory(&obj->m_TexTransform, sizeof(obj->m_TexTransform));
	}

	// Update InstanceData
	for (auto& prop : m_Maps[mapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->UpdateInstanceData(m_RItemsMap[prop], m_RItemsVec);
	}

	// Visible Off
	for (auto& itemInfo : mapInfoVector)
	{
		GameObject* obj = FindObject<GameObject>(itemInfo.meshName, std::to_string(itemInfo.typeID));
		obj->m_IsVisible = false;
		obj->m_IsVisibleOnePassCheck = false;
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

			user->m_IsVisible = isVisible;
			user->m_IsVisibleOnePassCheck = isVisible;
			user->m_hp = MAX_HP;
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

	for (auto& p : playerInfoVec) {
		if (userName == p.playerName)
		{
			user->InitializeTransform();
			user->SetAnimationKeyState(AnimationController::PlayerState::STATE_IDLE);
			user->SetAnimationPlayerState(AnimationController::PlayerState::STATE_IDLE);

			user->m_IsVisible = isVisible;
			user->m_IsVisibleOnePassCheck = isVisible;
			user->Rotate(0, XMConvertToRadians(p.rotY), 0);
			user->SetPosition(p.position);

			return;
		}
	}
}

void ApplicationContext::HiddenCharacter(Character* user)
{
	if (!user) return;

	user->m_IsVisible = false;
	user->m_IsVisibleOnePassCheck = false;
}

void ApplicationContext::HiddenCharacter(std::string userName)
{
	Character* user = FindObject<Character>(userName, userName);

	user->m_IsVisible = false;
	user->m_IsVisibleOnePassCheck = false;
}

void ApplicationContext::UpdateStateUI2D(std::string ui2dLayer, std::string ui2dName, int nowGauge)
{
	string ts = "GamePlay";
	UserInterface* ui = FindObject<UserInterface>(ui2dLayer + ts, ui2dName);
	if (!ui) return;

	float sizeX, sizeY;
	XMFLOAT2 pos;
	float posXGap;

	if (ui2dName == OBJECT_NAME_GAMEPLAY_HEALTH)
	{
		float sx = (float)(MAXGAUGE_SIZE_X * nowGauge) / MAX_HP;
		sizeX = sx * 15.f;
		sizeY = 45.f * 15.f;
		pos = XMFLOAT2(-730.f, -360.f);
		posXGap = pos.x - ((float)MAXGAUGE_SIZE_X / 2 * 1.5f) + (sx / 2 * 1.5f);
	}
	else if (ui2dName == OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE)
	{
		float sx = (float)(MAXGAUGE_SIZE_X * nowGauge) / MAX_ATTACKGAUGE;
		sizeX = sx * 15.f;
		sizeY = 45.f * 15.f;
		pos = XMFLOAT2(-730.f, -430.f);
		posXGap = pos.x - ((float)MAXGAUGE_SIZE_X / 2 * 1.5f) + (sx / 2 * 1.5f);
	}
	else if (ui2dName == OBJECT_NAME_GAMEPLAY_SKILL_GAUGE)
	{
		float sx = (float)(MAXGAUGE_SIZE_X * nowGauge) / MAX_SKILLGAUGE;
		sizeX = sx * 15.f;
		sizeY = 35.f * 15.f;
		pos = XMFLOAT2(-730.f, -495.f);
		posXGap = pos.x - ((float)MAXGAUGE_SIZE_X / 2 * 1.5f) + (sx / 2 * 1.5f);
	}
	else
	{
		return;
	}

	ui->m_PositionRatio = { ((posXGap - (sizeX / 20.f))), -(pos.y + (sizeY / 20.f)) };
	ui->m_SizeRatio = { sizeX / sizeY, sizeY / 1080.f };

	ui->SetPosition(pos.x, pos.y, 1.f);

	ui->m_UIPos = XMFLOAT2(pos.x, pos.y);
	ui->m_UISize = XMFLOAT2(sizeX * 0.1f, sizeY * 0.1f);
}

void ApplicationContext::UpdateLoadingBarUI2D(XMFLOAT2 nowPos)
{
	UserInterface* ui = FindObject<UserInterface>(OBJECT_TYPE_LOADING_BAR, OBJECT_NAME_LOBBY_LOADING_BAR);
	if (!ui) return;

	float sizeX = 88.f * 15.f;
	float sizeY = 62.f * 15.f;
	XMFLOAT2 pos = nowPos;

	ui->m_PositionRatio = { ((pos.x - (sizeX / 20.f))), -(pos.y + (sizeY / 20.f)) };
	ui->m_SizeRatio = { sizeX / sizeY, sizeY / 1080.f };

	ui->SetPosition(pos.x, pos.y, 1.f);

	ui->m_UIPos = XMFLOAT2(pos.x, pos.y);
	ui->m_UISize = XMFLOAT2(sizeX * 0.1f, sizeY * 0.1f);
}

void ApplicationContext::DisplayUI2D(std::string ui2dLayer, std::string ui2dName, XMFLOAT2 pos, XMFLOAT2 size, TextAlignType textAlignType, int zLayer, bool isText)
{
	UserInterface* ui = FindObject<UserInterface>(ui2dLayer, ui2dName);
	if (!ui) return;

	float sizeX = size.x * 15.f;
	float sizeY = size.y * 15.f;

	ui->InitializeTransform();
	ui->m_IsVisible = true;
	ui->m_IsVisibleOnePassCheck = true;
	ui->m_ZLayer = zLayer;
	ui->m_IsText = isText;

	switch (textAlignType)
	{
	case TextAlignType::LT:
		ui->SetTextAlignType(TextAlignType::LT);
		ui->m_PositionRatio = { WIDTH_NORMALIZE_LT((pos.x - (sizeX / 2.f))), HEIGHT_NORMALIZE_LT(pos.y + (sizeY / 2.f)) };
		ui->m_SizeRatio = { sizeX / sizeY, sizeY };

		break;
	case TextAlignType::Center:
		ui->SetTextAlignType(TextAlignType::Center);
		ui->m_PositionRatio = { ((pos.x - (sizeX / 20.f))), -(pos.y + (sizeY / 20.f)) };
		ui->m_SizeRatio = { sizeX / sizeY, sizeY / 1080.f };

		break;
	default:
		break;
	}

	ui->Scale(sizeX, sizeY, 1);
	ui->SetPosition(pos.x, pos.y, 1.f);

	ui->m_UIPos = XMFLOAT2(pos.x, pos.y);
	ui->m_UISize = XMFLOAT2(sizeX * 0.1f, sizeY * 0.1f);
}

void ApplicationContext::HiddenUI2D(std::string ui2dLayer, std::string ui2dName)
{
	UserInterface* ui = FindObject<UserInterface>(ui2dLayer, ui2dName);
	if (!ui) return;

	ZeroMemory(&ui->m_World, sizeof(ui->m_World));
	ZeroMemory(&ui->m_TexTransform, sizeof(ui->m_TexTransform));

	GraphicsContext::GetApp()->UpdateInstanceData(m_RItemsMap[ui2dLayer], m_RItemsVec);

	ui->m_IsVisible = false;
	ui->m_IsVisibleOnePassCheck = false;
}

void ApplicationContext::SetDisplayUI2D(std::string ui2dLayer, std::string ui2dName, bool isVisible)
{
	UserInterface* ui = FindObject<UserInterface>(ui2dLayer, ui2dName);
	if (!ui) return;

	ui->m_IsVisible = isVisible;
}

void ApplicationContext::SetPickingUI2D(std::string ui2dLayer, std::string ui2dName, bool isVisible)
{
	UserInterface* ui = FindObject<UserInterface>(ui2dLayer, ui2dName);
	if (!ui) return;
}

void ApplicationContext::CreateParticle(std::string particleName, std::string instID, std::string matName, bool isLoop, DirectX::XMFLOAT3 offset, float particlePlaySpeed)
{
	Particle* particle = CreateObject<Particle>(particleName, instID);
	particle->SetMesh(particleName, particleName);
	particle->m_MaterialIndex = AssertsReference::GetApp()->m_Materials[matName]->DiffuseSrvHeapIndex;
	particle->m_IsVisible = false;
	particle->m_IsVisibleOnePassCheck = false;
	particle->SetIsLoop(isLoop);
	particle->SetParticleOffset(offset);
	particle->SetParticlePlaySpeed(particlePlaySpeed);
}

void ApplicationContext::DisplayParticle(std::string particleName, std::string instID, DirectX::XMFLOAT3 pos, bool isVisible, bool isCapture)
{
	Particle* ptc = FindObject<Particle>(particleName, instID);
	if (!ptc) return;

	ptc->m_IsVisible = isVisible;
	ptc->m_IsVisibleOnePassCheck = isVisible;
	ptc->InitializeTransform();

	if (isCapture)
	{
		ptc->CapturePosition();
	}
	else
		ptc->SetPosition(pos);
	ptc->PlayParticle();
}

void ApplicationContext::HiddenParticle(std::string particleName, std::string instID)
{
	Particle* ptc = FindObject<Particle>(particleName, instID);
	if (!ptc) return;

	ZeroMemory(&ptc->m_World, sizeof(ptc->m_World));
	ZeroMemory(&ptc->m_TexTransform, sizeof(ptc->m_TexTransform));
	ptc->StopParticle();

	GraphicsContext::GetApp()->UpdateInstanceData(m_RItemsMap[particleName], m_RItemsVec);

	ptc->m_IsVisible = false;
	ptc->m_IsVisibleOnePassCheck = false;
}

void ApplicationContext::BulletReset()
{
	for (int i = 0; i < MAX_BULLET_COUNT; ++i)
	{
		m_ActiveBulletCheck[i] = false;
		m_ActiveBullet[i] = 0;
	}
	m_ActiveBulletCnt = 0;

	for (int i = 0; i < MAX_SKILL_BULLET_COUNT; ++i)
	{
		m_AtiveSkillBulletCheck[i] = false;
		m_AtiveSkillBullet[i] = 0;
	}
	m_AtiveSkillBulletCnt = 0;
}
