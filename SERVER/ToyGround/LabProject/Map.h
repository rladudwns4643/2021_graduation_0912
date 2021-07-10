#pragma once

// Map Container
// mapInfoVevctor: 맵에 배치된 모든 프롭 정보들
// playerVector: 맵에 배치된 모든 플레이어 정보들
// propTypeVector: 중복되는 것 없이 에셋이름만 모아놓은 컨테이너

class Map
{
public:
	Map() = default;
	~Map();

public:
	// 맵 정보들
	std::vector<MapTool::MapInfo> mapInfoVector;
	std::vector<MapTool::PlayerInfo> playerInfoVector;

	// propTypeVector: 중복되는 것 없이 에셋이름만 모아놓은 컨테이너
	// mapInfoChangeableVector : 변화가 가능한 벡터
	std::vector<std::string> propTypeVector;

	// 텍스쳐 이름
	int propTexture;
};

