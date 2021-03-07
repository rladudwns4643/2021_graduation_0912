#include "pch.h"
#include "ObjectInfo.h"

ObjectInfo::ObjectInfo(std::string instID, UINT instIndex) :
	m_InstanceCount(0),
	m_LifeCount(0)
{
	AddInstance(instID, instIndex);
}

void ObjectInfo::AddInstance(std::string instID, UINT instIndex)
{
	if (m_InstanceKeyMap.find(instID) != m_InstanceKeyMap.end()) {
		cout << "Duplicate Instance Key!" << endl;
	}
	else
	{
		m_InstanceKeyMap[instID] = instIndex;

		m_InstanceCount += 1;
		m_LifeCount += 1;
	}
}

UINT ObjectInfo::GetInstanceIndex(std::string instID)
{
	if (!m_InstanceKeyMap.count(instID)) {
		cout << "Inst: Error! None Key: " << instID << endl;
		return -1;
	}

	return m_InstanceKeyMap[instID];
}
