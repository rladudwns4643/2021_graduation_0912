#pragma once

class ObjectInfo
{
private:
	// 인스턴스 고유 ID / Index
	std::map<std::string, UINT> m_InstanceKeyMap;

public:
	ObjectInfo(std::string instID, UINT instIndex);
	~ObjectInfo() = default;

	void AddInstance(std::string instID, UINT instIndex);
	UINT GetInstanceIndex(std::string instID);

	std::map<std::string, UINT>& GetInstanceKeyMap() { return m_InstanceKeyMap; };

public:
	std::string m_Type; /*geo type(mesh 이름)*/

	UINT m_InstanceCount;

	// lifecount만큼 벡터에서 그려줌
	UINT m_LifeCount;
};