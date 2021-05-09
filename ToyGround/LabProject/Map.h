#pragma once

/* Map Container ���� */
// mapInfoVevctor: �ʿ� ��ġ�� ��� ���� ������
// playerVector: �ʿ� ��ġ�� ��� �÷��̾� ������
// propTypeVector: �ߺ��Ǵ� �� ���� �����̸��� ��Ƴ��� �����̳�

class Map
{
public:
	Map() = default;
	~Map();

public:
	// �� ������
	std::vector<MapTool::MapInfo> mapInfoVector;
	std::vector<MapTool::PlayerInfo> playerInfoVector;

	// propTypeVector: �ߺ��Ǵ� �� ���� �����̸��� ��Ƴ��� �����̳�
	std::vector<std::string> propTypeVector;

	// �ؽ��� �̸� ( ���� / ĳ���� 4���� )
	int propTexture;
};

