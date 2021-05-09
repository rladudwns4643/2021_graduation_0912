#pragma once

class ObjectInfo
{
private:
	// �ν��Ͻ� ���� ID / Index
	std::map<std::string, UINT> m_InstanceKeyMap;

public:
	ObjectInfo(std::string instID, UINT instIndex);
	~ObjectInfo() = default;

	void AddInstance(std::string instID, UINT instIndex);
	UINT GetInstanceIndex(std::string instID);

	std::map<std::string, UINT>& GetInstanceKeyMap() { return m_InstanceKeyMap; };

public:
	std::string m_Type; /*geo type(mesh �̸�)*/

	UINT m_InstanceCount;

	// lifecount��ŭ ���Ϳ��� �׷���
	UINT m_LifeCount;
};