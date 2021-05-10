#pragma once
#include "Object.h"

// ¸Ê Å¥ºê °³¼ö
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_DEPTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 5

class Map {
public:
	Map();
	~Map();

	void LoadMapInfo(string mapName);

public:
	int data[MAP_HEIGHT_BLOCK_NUM][MAP_DEPTH_BLOCK_NUM][MAP_WIDTH_BLOCK_NUM];
};