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
	//for (int i = 0; i < ID_COUNT; ++i) {
	//	size_t list_size = m_obj_list[i].size();
	//
	//	for (int j = 0; j < list_size; ++j) {
	//		if (m_obj_list) delete m_obj_list[i][j];
	//	}
	//}
}

void Map::LoadMapInfo(string mapName) {
	string path = "Maps\\" + mapName + ".txt";
	cout << path << endl;
	std::ifstream fileIn(path);
	
	//todo: 맵 파일에서 불러와서 해야함 당장 임시
	XMFLOAT4 t1{ 0.f, 0.f, -1500.f, 0.f };
	XMFLOAT4 t2{ 0.f, 0.f, 1500.f, 0.f };
	SR::g_spawn_pos.SetSpawnPosition(SpawnPosition::ePositionType::SPAWN_1P, t1);
	SR::g_spawn_pos.SetSpawnPosition(SpawnPosition::ePositionType::SPAWN_2P, t2);


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
					ri = MAP_DEPTH_BLOCK_NUM - i - 1;
					rj = MAP_WIDTH_BLOCK_NUM - j - 1;
					data[k][i][j] = input;
					data[k][ri][rj] = input;
				}
			}
		}
	}
	//for (int k = 0; k < MAP_HEIGHT_BLOCK_NUM; ++k) {
	//	for (int i = 0; i < MAP_DEPTH_BLOCK_NUM; ++i) {
	//		for (int j = 0; j < MAP_WIDTH_BLOCK_NUM; ++j) {
	//			cout << data[k][i][j];
	//		}
	//		cout << endl;
	//	}
	//	cout << endl;
	//}
}