#pragma once
#include "GameObject.h"

class CharacterParts : public GameObject
{
public:
	explicit CharacterParts(std::string type, std::string id);
	virtual ~CharacterParts();

public:
	std::string m_PartsName;

	std::string m_PartsBoneName;
	int			m_PartsBoneIndex;
};