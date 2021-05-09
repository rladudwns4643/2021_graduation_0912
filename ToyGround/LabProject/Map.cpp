#include "pch.h"
#include "Map.h"

Map::~Map()
{
	mapInfoVector.clear();
	playerInfoVector.clear();

	propTypeVector.clear();
}
