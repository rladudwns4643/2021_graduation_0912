#include "pch.h"
#include "extern.h"
#include "Map.h"

Map::Map() {

}

Map::~Map() {
	for (int i = 0; i < ID_COUNT; ++i) {
		size_t list_size = m_obj_list[i].size();

		for (int j = 0; j < list_size; ++j) {
			if (m_obj_list) delete m_obj_list[i][j];
		}
	}
}

void Map::LoadMap(int map_no) {
	for (int i = 0; i < ID_COUNT; ++i) {
		size_t list_size = SR::g_map.m_obj_list[i].size(); //SR::g_map[map_no].m_obj_list[i].size();
		m_obj_list[i].resize(list_size);

		for (int j = 0; j < list_size; ++j) {
			m_obj_list[i][j] = new Object(*SR::g_map.m_obj_list[i][j]);
		}
	}
}
