#pragma once
#include "Singleton.h"
#include "GameObject.h"
#include "ObjectInfo.h"

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
	XMFLOAT3	FindSpawnLocation(std::string mapName, int spawnLocation);

	void CreateSkycube(std::string skycubeName, std::string instID, std::string matName);
	void CreateProps(std::string mapName);
	void CreateCharacter(std::string meshName, std::string instID, std::string matName, int skinnedCBIndex);

	void DisplayProps(std::string mapName, bool isScale = false, float scaleValue = 1.f);

	void DisplayCharacter(std::string mapName, Character* user, int spawnLocation, bool isVisible = true);
	void DisplayCharacter(std::string mapName, std::string userName, int spawnLocation, bool isVisible = true);

public:
	std::map<std::string, ObjectInfo*> m_RItemsMap;
	std::vector<GameObject*> m_RItemsVec;
	std::map<std::string, Map*> m_Maps;
};

