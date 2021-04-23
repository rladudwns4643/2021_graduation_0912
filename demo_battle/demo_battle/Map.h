#pragma once
#include "Object.h"

class Map {
public:
	enum e_obj_list_type { FIXED, NOTFIXED, ABOVE, ID_COUNT };

public:
	Map();
	~Map();

	void LoadMap(int map_no);

public:
	std::vector<Object*> m_obj_list[ID_COUNT];
};