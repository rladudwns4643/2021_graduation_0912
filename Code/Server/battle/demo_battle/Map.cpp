#include "pch.h"
#include "extern.h"
#include "Map.h"

Map::Map() {
	for (int k = 0; k < MAP_HEIGHT_BLOCK_NUM; ++k) {
		for (int i = 0; i < MAP_DEPTH_BLOCK_NUM; ++i) {
			for (int j = 0; j < MAP_WIDTH_BLOCK_NUM; ++j) {
				data[k][i][j] = 0;
			}
		}
	}
}

Map::~Map() {
	m_obj_type.clear();
	for (int i = 0; i < eObjType::COUNT; ++i) {
		m_obj_list[i].clear();
	}
}

void Map::LoadMapInfo(string mapName) {
	string path = "Maps\\" + mapName + ".txt";
	cout << path << endl;
	std::ifstream fileIn(path);
	
	XMFLOAT4 t1{ 0.f, 0.f, -1500.f, 0.f };
	XMFLOAT4 t2{ 0.f, 0.f, 1500.f, 90.f };

	set<int> obj_type;
	
	if (fileIn) {
		int ri, rj;
		for (int k = 0; k < MAP_HEIGHT_BLOCK_NUM; ++k) {
			int floor;
			fileIn >> floor;
			for (int i = 0; i < MAP_DEPTH_BLOCK_NUM / 2 + 1; ++i) {
				for (int j = 0; j < MAP_WIDTH_BLOCK_NUM; ++j) {
					// 배열에 맵 저장
					int input;
					fileIn >> input;
					obj_type.insert(input);
					ri = MAP_DEPTH_BLOCK_NUM - i - 1;
					rj = MAP_WIDTH_BLOCK_NUM - j - 1;
					data[k][i][j] = input;
					data[k][ri][rj] = input;
				}
			}
		}
	}

	for (auto& i : obj_type) {
		m_obj_type.emplace_back(i);
	}
}