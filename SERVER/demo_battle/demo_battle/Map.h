#pragma once
#include "Object.h"

// ¸Ê Å¥ºê °³¼ö
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_DEPTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 5

struct MapInfo {
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	int type_id;
};
struct PlayerInfo {
	XMFLOAT3 position;
	XMFLOAT3 rotation;
};

class Map {
public:
	enum eObjType { FIXED, NONFIXED, COUNT };
public:
	Map();
	~Map();

	void LoadMapInfo(string mapName);

public:
	vector<Object*> m_obj_list[COUNT];
	vector<int> m_obj_type;
	int data[MAP_HEIGHT_BLOCK_NUM][MAP_DEPTH_BLOCK_NUM][MAP_WIDTH_BLOCK_NUM];
};