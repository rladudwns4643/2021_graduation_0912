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
	}

	template <class TObject>
	TObject* FindObject(std::string type, std::string instID)
	{
		if (m_RItemsMap[type]->GetInstanceIndex(instID) == -1)
			return nullptr;
		return dynamic_cast<TObject*>(m_RItemsVec[m_RItemsMap[type]->GetInstanceIndex(instID)]);
	}

public:
	void CreateProps(std::string mapName);
	void CreateContourProps(std::string mapName);
	void CreateCharacter(std::string meshName, std::string instID, std::string matName, int skinnedCBIndex /*Character 종류(역할)*/);
	
	void DisplayProps(std::string mapName, bool isScale = false, float scaleValue = 100.f);
	
	void DisplayCharacter(std::string mapName, Character* user, int spawnLocation, bool isVisible = true);
	void DisplayCharacter(std::string mapName, std::string userName, int spawnLocation, bool isVisible = true);
public:
	void CreateDebugBoundingBox(std::string boundsName, std::string boundsInstName);

public:
	std::map<std::string, ObjectInfo*> m_RItemsMap;
	std::vector<GameObject*> m_RItemsVec;
	std::map<std::string, Map*> m_Maps;

};

