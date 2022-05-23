#pragma once
#include "Singleton.h"
#include "GameObject.h"
#include "ObjectInfo.h"
#include "Service.h"

namespace Core {
	extern int g_DisplayWidth;
	extern int g_DisplayHeight;
}

class Map;
class Character;
class ApplicationContext : public TemplateSingleton<ApplicationContext>
{
public:
	template <class TObject>
	TObject* CreateObject(std::string type, std::string instID)
	{
		// 꼭 해야할것들
		// 자신의 Index가 정해짐
		GameObject* obj = new TObject(type, instID);
		m_RItemsVec.push_back(obj);

		// Map 제작
		// Map 중복체크
		if (m_RItemsMap.find(type) != m_RItemsMap.end())
		{
			//cout << "Create Instance!" << endl;
			// Instancemap 조정
			m_RItemsMap[type]->AddInstance(instID, obj->GetIndex());
		}
		else
		{
			// 새로운 타입(메쉬)이 추가됨
			// -> ObjInfo 세팅
			ObjectInfo* objInfo = new ObjectInfo(instID, obj->GetIndex());
			objInfo->m_Type = type;
			m_RItemsMap[type] = objInfo;
		}

		return dynamic_cast<TObject*>(obj);
	}

	template <class TObject>
	TObject* FindObject(std::string type, std::string instID)
	{
		if (!m_RItemsMap.count(type)) {
			cout << "Error! None Type" << endl;
			return nullptr;
		}

		if (m_RItemsMap[type]->GetInstanceIndex(instID) == -1)
			return nullptr;
		return dynamic_cast<TObject*>(m_RItemsVec[m_RItemsMap[type]->GetInstanceIndex(instID)]);
	}

public:
	std::string FindMapName(int mapCode) const;
	XMFLOAT3	FindSpawnLocation(std::string mapName, Character* user);
	XMFLOAT3	FindSpawnLocation(std::string mapName, std::string userName);

	void CreateSkycube(std::string skycubeName, std::string instID, std::string matName);
	void CreateProps(std::string mapName);
	void CreateUI2D(std::string ui2dLayer, std::string ui2dName, int matIndex, int uiPressedTextureIdx = -1, int uiDisabledTextureIdx = -1);
	void CreateCharacter(std::string meshName, std::string instID, std::string matName, int skinnedCBIndex);
	void CreateWeapon(std::string weaponName, std::string subWeaponName, std::string partName);

	void CreateGem();
	void DisplayGem(int instID, float posX, float posY, float posZ);
	void HiddenGem(int instID, bool isVisible = false);

	void CreateBullet();
	void UpdateBullet();
	void DisplayBullet(int instID, XMFLOAT3 startPos, XMFLOAT3 look, int firedPlayerID, int bulletNum = 1);
	void HiddenBullet(int instID, int bulletNum = 1);

	void DisplayProps(std::string mapName, bool isScale = false, float scaleValue = 1.f);
	void HiddenProps(std::string mapName);

	void DisplayCharacter(std::string mapName, Character* user, bool isVisible = true);
	void DisplayCharacter(std::string mapName, std::string userName, bool isVisible = true);
	void HiddenCharacter(Character* user);
	void HiddenCharacter(std::string userName);

	void UpdateStateUI2D(std::string ui2dLayer, std::string ui2dName, int nowGauge, bool addSceneName = true);

	void UpdateLoadingBarUI2D(XMFLOAT2 nowPos);

	void DisplayUI2D(std::string ui2dLayer, std::string ui2dName, XMFLOAT2 pos, XMFLOAT2 size, TextAlignType textAlignType = TextAlignType::NONE, int zLayer = -1, bool isText = false);
	void HiddenUI2D(std::string ui2dLayer, std::string ui2dName);
	void SetDisplayUI2D(std::string ui2dLayer, std::string ui2dName, bool isVisible);
	void SetPickingUI2D(std::string ui2dLayer, std::string ui2dName, bool isVisible);
	
	void CreateParticle(std::string particleName, std::string instID, std::string matName, bool isLoop, DirectX::XMFLOAT3 offset, float particlePlaySpeed = 1.f);
	void DisplayParticle(std::string particleName, std::string instID, DirectX::XMFLOAT3 pos, bool isVisible = false, bool isCapture = false);
	void HiddenParticle(std::string particleName, std::string instID);

	void BulletReset();

public:
	std::map<std::string, ObjectInfo*> m_RItemsMap;
	std::vector<GameObject*> m_RItemsVec;
	std::map<std::string, Map*> m_Maps;

	int m_MapArray[MAP_HEIGHT_BLOCK_NUM][MAP_DEPTH_BLOCK_NUM][MAP_WIDTH_BLOCK_NUM];
	XMINT2 m_WaterMap[MAP_WATER_NUM];

	int m_LastTypeId;

	// Bullet 관련
	bool m_ActiveBulletCheck[MAX_BULLET_COUNT] = { false };
	int m_ActiveBullet[MAX_BULLET_COUNT];
	int m_ActiveBulletCnt = 0;

	bool m_AtiveSkillBulletCheck[MAX_SKILL_BULLET_COUNT] = { false };
	int m_AtiveSkillBullet[MAX_SKILL_BULLET_COUNT];
	int m_AtiveSkillBulletCnt = 0;

	int shiftArr[81] = {
		 1,  1, -1,  0,  1, -1,  -1,  1, -1,
		 1,  1,  0,  0,  1,  0,  -1,  1,  0,
		 1,  1,  1,  0,  1,  1,  -1,  1,  1,

		 1,  0, -1,  0,  0, -1,  -1,  0, -1,
		 1,  0,  0,  0,  0,  0,  -1,  0,  0,
		 1,  0,  1,  0,  0,  1,  -1,  0,  1,

		 1, -1, -1,  0, -1, -1,  -1, -1, -1,
		 1, -1,  0,  0, -1,  0,  -1, -1,  0,
		 1, -1,  1,  0, -1,  1,  -1, -1,  1,
	};
	int shiftGemArr[81] = {
		 1,  0, -1,  0,  0, -1,  -1,  0, -1,
		 1,  0,  0,  0,  0,  0,  -1,  0,  0,
		 1,  0,  1,  0,  0,  1,  -1,  0,  1,
	};
};

