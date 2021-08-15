#include "pch.h"
#include "AssertsReference.h"
#include "GeometryGenerator.h"
#include "ApplicationContext.h"
#include "Map.h"

Map* AssertsReference::LoadMapInfo(string mapName)
{
	Map* map = new Map;
	set<string> propTypeSet;

	MapTool::PlayerInfo prePlayerInfo;
	prePlayerInfo.playerName = CHARACTER_COWBOY;
	prePlayerInfo.position.x = 0.f;
	prePlayerInfo.position.y = 0.f;
	prePlayerInfo.position.z = STD_CUBE_SIZE * 14;
	prePlayerInfo.rotY = 180.f;
	prePlayerInfo.spawnPos = XMFLOAT3(0, 0, STD_CUBE_SIZE * 14);
	map->playerInfoVector.emplace_back(prePlayerInfo);

	prePlayerInfo.playerName = CHARACTER_GUNMAN;
	prePlayerInfo.position.x = 0.f;
	prePlayerInfo.position.y = 0.f;
	prePlayerInfo.position.z = -STD_CUBE_SIZE * 14;
	prePlayerInfo.rotY = 0.f;
	prePlayerInfo.spawnPos = XMFLOAT3(0, 0, -STD_CUBE_SIZE * 14);
	map->playerInfoVector.emplace_back(prePlayerInfo);

	//-------------------------
	string path = "Maps\\" + mapName + ".txt";
	//cout << path << endl;
	std::ifstream fileIn(path);

	if (fileIn)
	{
		MapTool::MapInfo preInfo;
		int rk, ri, rj;
		int typeIDCount = 0;

		float startX = -STD_CUBE_SIZE * (MAP_WIDTH_BLOCK_NUM / 2);
		float startY = -STD_CUBE_SIZE;
		float startZ = -STD_CUBE_SIZE * (MAP_DEPTH_BLOCK_NUM / 2);
		float shiftX, shiftY, shiftZ;

		// Create Weapon
		propTypeSet.insert(OBJECT_MESH_STR_REVOLVER);
		for (int i = 0; i < 4; ++i)
		{
			preInfo.meshName = OBJECT_MESH_STR_REVOLVER;
			preInfo.rotation.x = 0.0f;
			preInfo.rotation.y = 0.0f;
			preInfo.rotation.z = 0.0f;
			preInfo.position.x = i * 100.f;
			preInfo.position.y = 200.0f;
			preInfo.position.z = 0.0f;
			preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;
			preInfo.typeID = ++typeIDCount;
			preInfo.isCulling = false;
			preInfo.canBroken = false;
			map->mapInfoVector.emplace_back(preInfo);
		}

		// Create Floor
		propTypeSet.insert(OBJECT_MESH_STR_FLOOR);
		preInfo.meshName = OBJECT_MESH_STR_FLOOR;
		preInfo.rotation.x = 0.0f;
		preInfo.rotation.y = 0.0f;
		preInfo.rotation.z = 0.0f;
		preInfo.position.x = 0.0f;
		preInfo.position.y = 0.0f;
		preInfo.position.z = 0.0f;
		preInfo.textureName = TEXTURE_STR_Floor_Texture;
		preInfo.typeID = ++typeIDCount;
		preInfo.isCulling = false;
		preInfo.canBroken = false;
		map->mapInfoVector.emplace_back(preInfo);

		// create Wall
		propTypeSet.insert(OBJECT_MESH_STR_WALL_21);
		preInfo.meshName = OBJECT_MESH_STR_WALL_21;
		preInfo.rotation.x = 0.0f;
		preInfo.rotation.y = 90.0f;
		preInfo.rotation.z = 0.0f;
		preInfo.position.x = 0.0f;
		preInfo.position.y = 0.0f;
		preInfo.position.z = -1690.f;
		preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;
		preInfo.typeID = ++typeIDCount;
		preInfo.isCulling = false;
		preInfo.canBroken = false;
		map->mapInfoVector.emplace_back(preInfo);

		preInfo.meshName = OBJECT_MESH_STR_WALL_21;
		preInfo.rotation.x = 0.0f;
		preInfo.rotation.y = 270.0f;
		preInfo.rotation.z = 0.0f;
		preInfo.position.x = 0.0f;
		preInfo.position.y = 0.0f;
		preInfo.position.z = 1690.f;
		preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;
		preInfo.typeID = ++typeIDCount;
		preInfo.isCulling = false;
		preInfo.canBroken = false;
		map->mapInfoVector.emplace_back(preInfo);

		propTypeSet.insert(OBJECT_MESH_STR_WALL_33);
		preInfo.meshName = OBJECT_MESH_STR_WALL_33;
		preInfo.rotation.x = 0.0f;
		preInfo.rotation.y = 180.0f;
		preInfo.rotation.z = 0.0f;
		preInfo.position.x = -1090.f;
		preInfo.position.y = 0.0f;
		preInfo.position.z = 0.0f;
		preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;
		preInfo.typeID = ++typeIDCount;
		preInfo.isCulling = false;
		preInfo.canBroken = false;
		map->mapInfoVector.emplace_back(preInfo);

		preInfo.meshName = OBJECT_MESH_STR_WALL_33;
		preInfo.rotation.x = 0.0f;
		preInfo.rotation.y = 0.0f;
		preInfo.rotation.z = 0.0f;
		preInfo.position.x = 1090.f;
		preInfo.position.y = 0.0f;
		preInfo.position.z = 0.0f;
		preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;
		preInfo.typeID = ++typeIDCount;
		preInfo.isCulling = false;
		preInfo.canBroken = false;
		map->mapInfoVector.emplace_back(preInfo);
		 
		// Create Attack_Box 
		propTypeSet.insert(OBJECT_MESH_STR_ATTACK_BOX);
		preInfo.meshName = OBJECT_MESH_STR_ATTACK_BOX;
		preInfo.rotation.x = 0.0f;
		preInfo.rotation.y = 0.0f;
		preInfo.rotation.z = 0.0f;
		preInfo.position.x = 0.0f;
		preInfo.position.y = 0.0f;
		preInfo.position.z = 0.0f;
		preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;
		preInfo.typeID = ++typeIDCount;
		preInfo.isCulling = false;
		preInfo.canBroken = false;
		map->mapInfoVector.emplace_back(preInfo);

		// Gem
		propTypeSet.insert(OBJECT_MESH_STR_GEM);
		propTypeSet.insert(OBJECT_MESH_STR_BULLET_01);
		propTypeSet.insert(OBJECT_MESH_STR_BULLET_02);

		int waterCount = 0;
		for (int k = 0; k < MAP_HEIGHT_BLOCK_NUM; ++k)
		{
			int floor;
			fileIn >> floor;
			//cout << floor << endl;
			for (int i = 0; i < MAP_DEPTH_BLOCK_NUM / 2 + 1; ++i)
			{
				for (int j = 0; j < MAP_WIDTH_BLOCK_NUM; ++j)
				{
					// 배열에 맵 저장
					int input;
					fileIn >> input;
					//cout << input << " ";

					rk = k;
					ri = MAP_DEPTH_BLOCK_NUM - i - 1;
					rj = MAP_WIDTH_BLOCK_NUM - j - 1;

					// 물 체크
					if (k == 0 )
					{
						if (input == 4)
						{
							XMINT2 t{ i, j };
							AppContext->m_WaterMap[waterCount++] = t;
							if (i < MAP_DEPTH_BLOCK_NUM / 2)
							{
								XMINT2 rt{ ri, rj };
								AppContext->m_WaterMap[waterCount++] = rt;
							}
						}
						continue;
					}
					shiftX = 0.f;
					shiftY = 0.f;
					shiftZ = 0.f;

					// 오브젝트 생성
					switch (input / 10)
					{
					case 0:
					case 1:
					{
						preInfo.rotation.y = 0.f;
						switch (input)
						{
						case 1:
							{
								preInfo.meshName = OBJECT_MESH_STR_CUBE_01;
								propTypeSet.insert(OBJECT_MESH_STR_CUBE_01);
								preInfo.colWithChar = true;
								preInfo.isCulling = true;
								preInfo.canBroken = true;
							}
							break;
						case 2:
							{
								preInfo.meshName = OBJECT_MESH_STR_CUBE_02;
								propTypeSet.insert(OBJECT_MESH_STR_CUBE_02);
								preInfo.colWithChar = true;
								preInfo.isCulling = true;
								preInfo.canBroken = true;
							}
							break;
						case 3:
							{
								preInfo.meshName = OBJECT_MESH_STR_CUBE_03;
								propTypeSet.insert(OBJECT_MESH_STR_CUBE_03);
								preInfo.colWithChar = true;
								preInfo.isCulling = true;
								preInfo.canBroken = true;
							}
							break;
						case 4:
							{
								preInfo.meshName = OBJECT_MESH_STR_CUBE_04;
								propTypeSet.insert(OBJECT_MESH_STR_CUBE_04);
								preInfo.colWithChar = true;
								preInfo.isCulling = true;
								preInfo.canBroken = true;
							}
							break;
						case 5:
							preInfo.meshName = OBJECT_MESH_STR_PLANT;
							propTypeSet.insert(OBJECT_MESH_STR_PLANT);
							preInfo.colWithChar = false;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 6:
							preInfo.meshName = OBJECT_MESH_STR_TILE_01;
							propTypeSet.insert(OBJECT_MESH_STR_TILE_01);
							preInfo.colWithChar = false;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 7:
							preInfo.meshName = OBJECT_MESH_STR_TILE_02;
							propTypeSet.insert(OBJECT_MESH_STR_TILE_02);
							preInfo.colWithChar = false;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 8:
							preInfo.meshName = OBJECT_MESH_STR_TREE_01;
							propTypeSet.insert(OBJECT_MESH_STR_TREE_01);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 9:
							preInfo.meshName = OBJECT_MESH_STR_TREE_02;
							propTypeSet.insert(OBJECT_MESH_STR_TREE_02);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 10:
							preInfo.meshName = OBJECT_MESH_STR_BARREL;
							propTypeSet.insert(OBJECT_MESH_STR_BARREL);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 11:
							preInfo.meshName = OBJECT_MESH_STR_CHEST_DOWN;
							propTypeSet.insert(OBJECT_MESH_STR_CHEST_DOWN);
							preInfo.colWithChar = true;
							preInfo.rotation.y = 90.f;
							preInfo.isCulling = true;
							preInfo.canBroken = false;
							break;
						case 12:
							preInfo.meshName = OBJECT_MESH_STR_CHEST_UP;
							propTypeSet.insert(OBJECT_MESH_STR_CHEST_UP);
							shiftY = -STD_CUBE_SIZE / 2;
							shiftX = -STD_CUBE_SIZE * 0.25;
							preInfo.rotation.y = 90.f;
							preInfo.colWithChar = false;
							preInfo.isCulling = true;
							preInfo.canBroken = false;
							break;
						}
						break;
					}
					case 2:
					{
						if (input % 2)
							preInfo.rotation.y = 0.f;
						else
							preInfo.rotation.y = 90.f;
						preInfo.meshName = OBJECT_MESH_STR_CUBE_BRIDGE;
						propTypeSet.insert(OBJECT_MESH_STR_CUBE_BRIDGE);
						preInfo.colWithChar = true;
						shiftY = -STD_CUBE_SIZE * 0.85;
						preInfo.isCulling = true;
						preInfo.canBroken = true;
						break;
					}
					case 3:
					{
						if (input % 2)
							preInfo.rotation.y = 0.f;
						else
							preInfo.rotation.y = 90.f;
						preInfo.meshName = OBJECT_MESH_STR_LOGS;
						propTypeSet.insert(OBJECT_MESH_STR_LOGS);
						preInfo.colWithChar = true;
						preInfo.isCulling = true;
						preInfo.canBroken = true;
						break;
					}
					case 4:
					{
						if (input % 2)
							preInfo.rotation.y = 90.f;
						else
							preInfo.rotation.y = 0.f;
						preInfo.meshName = OBJECT_MESH_STR_GRAVESTONE;
						propTypeSet.insert(OBJECT_MESH_STR_GRAVESTONE);
						preInfo.colWithChar = true;
						preInfo.isCulling = true;
						preInfo.canBroken = false;
						break;
					}
					case 5:
					{
						switch (input)
						{
						case 50:
							preInfo.rotation.y = 0.f;
							shiftZ = -(STD_CUBE_SIZE / 2 - 5.0f);
							preInfo.meshName = OBJECT_MESH_STR_FENCE_01;
							propTypeSet.insert(OBJECT_MESH_STR_FENCE_01);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 51:
							preInfo.rotation.y = 90.f;
							shiftX = (STD_CUBE_SIZE / 2 - 5.0f);
							preInfo.meshName = OBJECT_MESH_STR_FENCE_02;
							propTypeSet.insert(OBJECT_MESH_STR_FENCE_02);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 52:
							preInfo.rotation.y = 0.f;
							shiftZ = (STD_CUBE_SIZE / 2 - 5.0f);
							preInfo.meshName = OBJECT_MESH_STR_FENCE_01;
							propTypeSet.insert(OBJECT_MESH_STR_FENCE_01);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						case 53:
							preInfo.rotation.y = 90.f;
							shiftX = -(STD_CUBE_SIZE / 2 - 5.0f);
							preInfo.meshName = OBJECT_MESH_STR_FENCE_02;
							propTypeSet.insert(OBJECT_MESH_STR_FENCE_02);
							preInfo.colWithChar = true;
							preInfo.isCulling = true;
							preInfo.canBroken = true;
							break;
						}
						break;
					}
					}
					if (input != 0)
					{
						preInfo.rotation.x = 0.f;
						preInfo.rotation.z = 0.f;
						preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;

						// 여기 수정
						preInfo.position.x = startX + (STD_CUBE_SIZE * j) + shiftX;
						preInfo.position.y = startY + (STD_CUBE_SIZE * k) + shiftY;
						preInfo.position.z = startZ + (STD_CUBE_SIZE * i) + shiftZ;
						preInfo.typeID = ++typeIDCount;
						map->mapInfoVector.emplace_back(preInfo);
						AppContext->m_MapArray[k][i][j] = typeIDCount;

						if (i < MAP_DEPTH_BLOCK_NUM / 2)
						{
							preInfo.position.x = -(startX + (STD_CUBE_SIZE * j)) - shiftX;
							preInfo.position.y = startY + (STD_CUBE_SIZE * k) + shiftY;
							preInfo.position.z = -(startZ + (STD_CUBE_SIZE * i)) - shiftZ;
							preInfo.typeID = ++typeIDCount;
							map->mapInfoVector.emplace_back(preInfo);
							AppContext->m_MapArray[rk][ri][rj] = typeIDCount;
						}
					}
				}
			}
		}
		AppContext->m_LastTypeId = typeIDCount;
	}

	// propTypeVector: 중복되는 것 없이 에셋이름만 모아놓은
	for (auto iter = propTypeSet.begin(); iter != propTypeSet.end(); ++iter)
	{
		map->propTypeVector.emplace_back(*iter);
		m_propTypeVector.emplace_back(*iter);
	}
	CreateBB();

	return map;
}

void AssertsReference::CreateBB()
{
	// Prop
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, STD_CUBE_SIZE, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_01] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, STD_CUBE_SIZE, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_02] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, STD_CUBE_SIZE, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_03] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, STD_CUBE_SIZE, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_04] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, STD_CUBE_SIZE, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, 0.15f, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_PLAT_01] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, STD_CUBE_SIZE, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, 0.15, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_PLAT_02] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, STD_CUBE_SIZE, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, 0.15, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_PLAT_03] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, STD_CUBE_SIZE, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, 0.15, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_PLAT_04] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(2100.f, 0.f, 3300.f);
		m_PropBoundingBox[OBJECT_MESH_STR_FLOOR] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(2100.f, 550.f, 100.f);
		m_PropBoundingBox[OBJECT_MESH_STR_WALL_21] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(100.f, 550.f, 3300.f);
		m_PropBoundingBox[OBJECT_MESH_STR_WALL_33] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, STD_CUBE_SIZE * 0.85, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, STD_CUBE_SIZE * 0.15, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CUBE_BRIDGE] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(45.f, 20.f, 45.f);
		m_PropBoundingBox[OBJECT_MESH_STR_PLANT] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(80.f, 5.f, 80.f);
		m_PropBoundingBox[OBJECT_MESH_STR_TILE_01] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(80.f, 5.f, 80.f);
		m_PropBoundingBox[OBJECT_MESH_STR_TILE_02] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(35.f, 320.f, 35.f);
		m_PropBoundingBox[OBJECT_MESH_STR_TREE_01] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(35.f, 320.f, 35.f);
		m_PropBoundingBox[OBJECT_MESH_STR_TREE_02] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(80.f, STD_CUBE_SIZE, 80.f);
		m_PropBoundingBox[OBJECT_MESH_STR_BARREL] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(60.f, 50.f, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CHEST_DOWN] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(15.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(60.f, 65.f, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_CHEST_UP] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(STD_CUBE_SIZE, 50.f, 35.f);
		m_PropBoundingBox[OBJECT_MESH_STR_FENCE_01] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(35.f, 60.f, STD_CUBE_SIZE);
		m_PropBoundingBox[OBJECT_MESH_STR_FENCE_02] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(85.f, 85.f, 30.f);
		m_PropBoundingBox[OBJECT_MESH_STR_GRAVESTONE] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(85.f, 55.f, 65.f);
		m_PropBoundingBox[OBJECT_MESH_STR_LOGS] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(0.f, 0.f, 0.f);
		m_PropBoundingBox[OBJECT_MESH_STR_REVOLVER] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 30.f, 0.f);
		bb->Extents = XMFLOAT3(50.f, 1000.f, 50.f);
		m_PropBoundingBox[OBJECT_MESH_STR_ATTACK_BOX] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, 0.f, 0.f);
		bb->Extents = XMFLOAT3(20.f, 40.f, 20.f);
		m_PropBoundingBox[OBJECT_MESH_STR_GEM] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, -19.f, 0.f);
		bb->Extents = XMFLOAT3(38.f, 38.f, 38.f);
		m_PropBoundingBox[OBJECT_MESH_STR_BULLET_01] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.f, -19.f, 0.f);
		bb->Extents = XMFLOAT3(38.f, 38.f, 38.f);
		m_PropBoundingBox[OBJECT_MESH_STR_BULLET_02] = std::move(bb);
	}

	// Character
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bb->Extents = XMFLOAT3(PLAYER_SIZE_X, PLAYER_SIZE_Y, PLAYER_SIZE_Z);
		m_PropBoundingBox[CHARACTER_COWBOY] = std::move(bb);
	}
	{
		auto bb = make_unique<BoundingBox>();
		bb->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bb->Extents = XMFLOAT3(PLAYER_SIZE_X, PLAYER_SIZE_Y, PLAYER_SIZE_Z);
		m_PropBoundingBox[CHARACTER_GUNMAN] = std::move(bb);
	}
}

void AssertsReference::BuildMaterials()
{
	auto sky = std::make_unique<Material>();
	sky->MatCBIndex = TEXTURE_INDEX_SkyBox;
	sky->DiffuseSrvHeapIndex = TEXTURE_INDEX_SkyBox;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;

	auto Cartoon_CubeWorld_Texture = std::make_unique<Material>();
	Cartoon_CubeWorld_Texture->MatCBIndex = TEXTURE_INDEX_Cartoon_CubeWorld_Texture;
	Cartoon_CubeWorld_Texture->DiffuseSrvHeapIndex = TEXTURE_INDEX_Cartoon_CubeWorld_Texture;
	Cartoon_CubeWorld_Texture->DiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	Cartoon_CubeWorld_Texture->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	Cartoon_CubeWorld_Texture->Roughness = 0.987734f;

	auto PolygonMinis_Texture_01_A = std::make_unique<Material>();
	PolygonMinis_Texture_01_A->MatCBIndex = TEXTURE_INDEX_PolygonMinis_Texture_01_A;
	PolygonMinis_Texture_01_A->DiffuseSrvHeapIndex = TEXTURE_INDEX_PolygonMinis_Texture_01_A;
	PolygonMinis_Texture_01_A->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	PolygonMinis_Texture_01_A->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	PolygonMinis_Texture_01_A->Roughness = 0.987734f;

	auto Polygon_Plane_Texture_02 = std::make_unique<Material>();
	Polygon_Plane_Texture_02->MatCBIndex = TEXTURE_INDEX_Polygon_Plane_Texture_02;
	Polygon_Plane_Texture_02->DiffuseSrvHeapIndex = TEXTURE_INDEX_Polygon_Plane_Texture_02;
	Polygon_Plane_Texture_02->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Polygon_Plane_Texture_02->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	Polygon_Plane_Texture_02->Roughness = 0.717734;

	auto Polygon_Plane_Texture_03 = std::make_unique<Material>();
	Polygon_Plane_Texture_03->MatCBIndex = TEXTURE_INDEX_Polygon_Plane_Texture_03;
	Polygon_Plane_Texture_03->DiffuseSrvHeapIndex = TEXTURE_INDEX_Polygon_Plane_Texture_03;
	Polygon_Plane_Texture_03->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Polygon_Plane_Texture_03->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	Polygon_Plane_Texture_03->Roughness = 0.717734;

	auto Floor_Texture = std::make_unique<Material>();
	Floor_Texture->MatCBIndex = TEXTURE_INDEX_Floor_Texture;
	Floor_Texture->DiffuseSrvHeapIndex = TEXTURE_INDEX_Floor_Texture;
	Floor_Texture->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Floor_Texture->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	Floor_Texture->Roughness = 0.717734;

	//---------------------------------------------------------------------------------------------
	auto MATCHINGROOM_BACKGROUND = std::make_unique<Material>();
	MATCHINGROOM_BACKGROUND->MatCBIndex = TEXTURE_INDEX_UI_MATCHINGROOM_BACKGROUND;
	MATCHINGROOM_BACKGROUND->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_MATCHINGROOM_BACKGROUND;
	MATCHINGROOM_BACKGROUND->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MATCHINGROOM_BACKGROUND->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MATCHINGROOM_BACKGROUND->Roughness = 0.9;

	auto MATCHINGROOM_LOGO_TOYGROUND = std::make_unique<Material>();
	MATCHINGROOM_LOGO_TOYGROUND->MatCBIndex = TEXTURE_INDEX_UI_MATCHINGROOM_LOGO_TOYGROUND;
	MATCHINGROOM_LOGO_TOYGROUND->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_MATCHINGROOM_LOGO_TOYGROUND;
	MATCHINGROOM_LOGO_TOYGROUND->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MATCHINGROOM_LOGO_TOYGROUND->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MATCHINGROOM_LOGO_TOYGROUND->Roughness = 0.9;

	auto MATCHINGROOM_READY1_RELEASED = std::make_unique<Material>();
	MATCHINGROOM_READY1_RELEASED->MatCBIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY1_RELEASED;
	MATCHINGROOM_READY1_RELEASED->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY1_RELEASED;
	MATCHINGROOM_READY1_RELEASED->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MATCHINGROOM_READY1_RELEASED->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MATCHINGROOM_READY1_RELEASED->Roughness = 0.9;

	auto MATCHINGROOM_READY1_PRESSED = std::make_unique<Material>();
	MATCHINGROOM_READY1_PRESSED->MatCBIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY1_PRESSED;
	MATCHINGROOM_READY1_PRESSED->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY1_PRESSED;
	MATCHINGROOM_READY1_PRESSED->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MATCHINGROOM_READY1_PRESSED->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MATCHINGROOM_READY1_PRESSED->Roughness = 0.9;

	auto MATCHINGROOM_READY2_RELEASED = std::make_unique<Material>();
	MATCHINGROOM_READY2_RELEASED->MatCBIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY2_RELEASED;
	MATCHINGROOM_READY2_RELEASED->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY2_RELEASED;
	MATCHINGROOM_READY2_RELEASED->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MATCHINGROOM_READY2_RELEASED->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MATCHINGROOM_READY2_RELEASED->Roughness = 0.9;

	auto MATCHINGROOM_READY2_PRESSED = std::make_unique<Material>();
	MATCHINGROOM_READY2_PRESSED->MatCBIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY2_PRESSED;
	MATCHINGROOM_READY2_PRESSED->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_MATCHINGROOM_READY2_PRESSED;
	MATCHINGROOM_READY2_PRESSED->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MATCHINGROOM_READY2_PRESSED->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	MATCHINGROOM_READY2_PRESSED->Roughness = 0.9;

	//--------------------------------------------------------------------------------------------
	auto LOBBY_BACKGROUND = std::make_unique<Material>();
	LOBBY_BACKGROUND->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_BACKGROUND;
	LOBBY_BACKGROUND->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_BACKGROUND;
	LOBBY_BACKGROUND->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_BACKGROUND->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_BACKGROUND->Roughness = 0.9;

	auto LOBBY_LOGO_TOYGROUND = std::make_unique<Material>();
	LOBBY_LOGO_TOYGROUND->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_LOGO_TOYGROUND;
	LOBBY_LOGO_TOYGROUND->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_LOGO_TOYGROUND;
	LOBBY_LOGO_TOYGROUND->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_LOGO_TOYGROUND->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_LOGO_TOYGROUND->Roughness = 0.9;

	auto LOBBY_LOGINUI = std::make_unique<Material>();
	LOBBY_LOGINUI->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_LOGINUI;
	LOBBY_LOGINUI->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_LOGINUI;
	LOBBY_LOGINUI->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_LOGINUI->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_LOGINUI->Roughness = 0.9;

	auto LOBBY_ID_INPUTATIVATE = std::make_unique<Material>();
	LOBBY_ID_INPUTATIVATE->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_ID_INPUTATIVATE;
	LOBBY_ID_INPUTATIVATE->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_ID_INPUTATIVATE;
	LOBBY_ID_INPUTATIVATE->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_ID_INPUTATIVATE->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_ID_INPUTATIVATE->Roughness = 0.9;

	auto LOBBY_PW_INPUTATIVATE = std::make_unique<Material>();
	LOBBY_PW_INPUTATIVATE->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_PW_INPUTATIVATE;
	LOBBY_PW_INPUTATIVATE->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_PW_INPUTATIVATE;
	LOBBY_PW_INPUTATIVATE->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_PW_INPUTATIVATE->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_PW_INPUTATIVATE->Roughness = 0.9;

	auto LOBBY_LOADINGUI = std::make_unique<Material>();
	LOBBY_LOADINGUI->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_LOADINGUI;
	LOBBY_LOADINGUI->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_LOADINGUI;
	LOBBY_LOADINGUI->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_LOADINGUI->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_LOADINGUI->Roughness = 0.9;

	auto LOBBY_LOADING_BAR = std::make_unique<Material>();
	LOBBY_LOADING_BAR->MatCBIndex = TEXTURE_INDEX_UI_LOBBY_LOADING_BAR;
	LOBBY_LOADING_BAR->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_LOBBY_LOADING_BAR;
	LOBBY_LOADING_BAR->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	LOBBY_LOADING_BAR->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LOBBY_LOADING_BAR->Roughness = 0.9;

	//---------------------------------------------------------------------------------
	auto GAMEPLAY_AIM = std::make_unique<Material>();
	GAMEPLAY_AIM->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_AIM;
	GAMEPLAY_AIM->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_AIM;
	GAMEPLAY_AIM->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_AIM->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_AIM->Roughness = 0.9;

	auto GAMEPLAY_PLAYER1_SCORE = std::make_unique<Material>();
	GAMEPLAY_PLAYER1_SCORE->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_PLAYER1_SCORE;
	GAMEPLAY_PLAYER1_SCORE->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_PLAYER1_SCORE;
	GAMEPLAY_PLAYER1_SCORE->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_PLAYER1_SCORE->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_PLAYER1_SCORE->Roughness = 0.9;

	auto GAMEPLAY_PLAYER2_SCORE = std::make_unique<Material>();
	GAMEPLAY_PLAYER2_SCORE->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_PLAYER2_SCORE;
	GAMEPLAY_PLAYER2_SCORE->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_PLAYER2_SCORE;
	GAMEPLAY_PLAYER2_SCORE->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_PLAYER2_SCORE->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_PLAYER2_SCORE->Roughness = 0.9;

	auto GAMEPLAY_TIMER = std::make_unique<Material>();
	GAMEPLAY_TIMER->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_TIMER;
	GAMEPLAY_TIMER->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_TIMER;
	GAMEPLAY_TIMER->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_TIMER->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_TIMER->Roughness = 0.9;

	auto GAMEPLAY_HEALTH = std::make_unique<Material>();
	GAMEPLAY_HEALTH->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_HEALTH;
	GAMEPLAY_HEALTH->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_HEALTH;
	GAMEPLAY_HEALTH->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_HEALTH->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_HEALTH->Roughness = 0.9;

	auto GAMEPLAY_ATTACK_GAUGE = std::make_unique<Material>();
	GAMEPLAY_ATTACK_GAUGE->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_ATTACK_GAUGE;
	GAMEPLAY_ATTACK_GAUGE->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_ATTACK_GAUGE;
	GAMEPLAY_ATTACK_GAUGE->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_ATTACK_GAUGE->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_ATTACK_GAUGE->Roughness = 0.9;

	auto GAMEPLAY_SKILL_GAUGE = std::make_unique<Material>();
	GAMEPLAY_SKILL_GAUGE->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_SKILL_GAUGE;
	GAMEPLAY_SKILL_GAUGE->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_SKILL_GAUGE;
	GAMEPLAY_SKILL_GAUGE->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_SKILL_GAUGE->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_SKILL_GAUGE->Roughness = 0.9;

	auto GAMEPLAY_STATE_FRONT = std::make_unique<Material>();
	GAMEPLAY_STATE_FRONT->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_STATE_FRONT;
	GAMEPLAY_STATE_FRONT->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_STATE_FRONT;
	GAMEPLAY_STATE_FRONT->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_STATE_FRONT->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_STATE_FRONT->Roughness = 0.9;

	auto GAMEPLAY_STATE_BACK = std::make_unique<Material>();
	GAMEPLAY_STATE_BACK->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_STATE_BACK;
	GAMEPLAY_STATE_BACK->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_STATE_BACK;
	GAMEPLAY_STATE_BACK->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_STATE_BACK->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_STATE_BACK->Roughness = 0.9;

	auto GAMEPLAY_WINNERBOARD = std::make_unique<Material>();
	GAMEPLAY_WINNERBOARD->MatCBIndex = TEXTURE_INDEX_UI_GAMEPLAY_WINNERBOARD;
	GAMEPLAY_WINNERBOARD->DiffuseSrvHeapIndex = TEXTURE_INDEX_UI_GAMEPLAY_WINNERBOARD;
	GAMEPLAY_WINNERBOARD->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	GAMEPLAY_WINNERBOARD->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	GAMEPLAY_WINNERBOARD->Roughness = 0.9;

	m_Materials["SkyBox"] = std::move(sky);
	m_Materials[TEXTURE_STR_Cartoon_CubeWorld_Texture] = std::move(Cartoon_CubeWorld_Texture);
	m_Materials[TEXTURE_STR_PolygonMinis_Texture_01_A] = std::move(PolygonMinis_Texture_01_A);
	m_Materials[TEXTURE_STR_Polygon_Plane_Texture_02] = std::move(Polygon_Plane_Texture_02);
	m_Materials[TEXTURE_STR_Polygon_Plane_Texture_03] = std::move(Polygon_Plane_Texture_03);
	m_Materials[TEXTURE_STR_Floor_Texture] = std::move(Floor_Texture);

	m_Materials[TEXTURE_STR_UI_MATCHINGROOM_BACKGROUND] = std::move(MATCHINGROOM_BACKGROUND);
	m_Materials[TEXTURE_STR_UI_MATCHINGROOM_LOGO_TOYGROUND] = std::move(MATCHINGROOM_LOGO_TOYGROUND);
	m_Materials[TEXTURE_STR_UI_MATCHINGROOM_READY1_BUTTON_RELEASED] = std::move(MATCHINGROOM_READY1_RELEASED);
	m_Materials[TEXTURE_STR_UI_MATCHINGROOM_READY1_BUTTON_PRESSED] = std::move(MATCHINGROOM_READY1_PRESSED);
	m_Materials[TEXTURE_STR_UI_MATCHINGROOM_READY2_BUTTON_RELEASED] = std::move(MATCHINGROOM_READY2_RELEASED);
	m_Materials[TEXTURE_STR_UI_MATCHINGROOM_READY2_BUTTON_PRESSED] = std::move(MATCHINGROOM_READY2_PRESSED);

	m_Materials[TEXTURE_STR_UI_LOBBY_BACKGROUND] = std::move(LOBBY_BACKGROUND);
	m_Materials[TEXTURE_STR_UI_LOBBY_LOGO_TOYGROUND] = std::move(LOBBY_LOGO_TOYGROUND);
	m_Materials[TEXTURE_STR_UI_LOBBY_LOGINUI] = std::move(LOBBY_LOGINUI);
	m_Materials[TEXTURE_STR_UI_LOBBY_ID_INPUTATIVATE] = std::move(LOBBY_ID_INPUTATIVATE);
	m_Materials[TEXTURE_STR_UI_LOBBY_PW_INPUTATIVATE] = std::move(LOBBY_PW_INPUTATIVATE);
	m_Materials[TEXTURE_STR_UI_LOBBY_LOADINGUI] = std::move(LOBBY_LOADINGUI);
	m_Materials[TEXTURE_STR_UI_LOBBY_LOADING_BAR] = std::move(LOBBY_LOADING_BAR);

	m_Materials[TEXTURE_STR_UI_GAMEPLAY_AIM] = std::move(GAMEPLAY_AIM);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_PLAYER1_SCORE] = std::move(GAMEPLAY_PLAYER1_SCORE);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_PLAYER2_SCORE] = std::move(GAMEPLAY_PLAYER2_SCORE);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_TIMER] = std::move(GAMEPLAY_TIMER);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_HEALTH] = std::move(GAMEPLAY_HEALTH);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_ATTACK_GAUGE] = std::move(GAMEPLAY_ATTACK_GAUGE);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_SKILL_GAUGE] = std::move(GAMEPLAY_SKILL_GAUGE);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_STATE_FRONT] = std::move(GAMEPLAY_STATE_FRONT);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_STATE_BACK] = std::move(GAMEPLAY_STATE_BACK);
	m_Materials[TEXTURE_STR_UI_GAMEPLAY_WINNERBOARD] = std::move(GAMEPLAY_WINNERBOARD);
}

void AssertsReference::BuildGeoMeshes(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 2);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(0.1f, 0.1f, 2, 2);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 2);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 2);

	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
	UINT quadVertexOffset = cylinderVertexOffset + (UINT)cylinder.Vertices.size();

	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();
	UINT quadIndexOffset = cylinderIndexOffset + (UINT)cylinder.Indices32.size();

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	// Box Bounds
	for (auto& p : box.Vertices)
	{
		XMVECTOR P = XMLoadFloat3(&p.Pos);

		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	BoundingBox boxbounds;
	XMStoreFloat3(&boxbounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&boxbounds.Extents, 0.5f * (vMax - vMin));

	// Grid Bounds
	for (auto& p : grid.Vertices)
	{
		XMVECTOR P = XMLoadFloat3(&p.Pos);

		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	BoundingBox gridbounds;
	XMStoreFloat3(&gridbounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&gridbounds.Extents, 0.5f * (vMax - vMin));

	// Sphere Bounds
	for (auto& p : sphere.Vertices)
	{
		XMVECTOR P = XMLoadFloat3(&p.Pos);

		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	BoundingBox spherebounds;
	XMStoreFloat3(&spherebounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&spherebounds.Extents, 0.5f * (vMax - vMin));

	// Submesh
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;
	boxSubmesh.Bounds = boxbounds;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;
	gridSubmesh.Bounds = gridbounds;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
	sphereSubmesh.Bounds = spherebounds;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Pos;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexC = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Pos;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexC = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Pos;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexC = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Pos;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexC = cylinder.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<GeometryMesh>();

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs[MESH_GEOID_RECT] = boxSubmesh;
	geo->DrawArgs[MESH_GEOID_GRID] = gridSubmesh;
	geo->DrawArgs[MESH_GEOID_SPHERE] = sphereSubmesh;
	geo->DrawArgs[MESH_GEOID_CYLINDER] = cylinderSubmesh;

	m_GeometryMesh[MESH_GEOID] = std::move(geo);
}

void AssertsReference::BuildBoundingBoxMeshes(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	auto bb = std::make_unique<BoundingBoxMesh>();

	//직육면체는 꼭지점(정점)이 8개이다.
	UINT VerticesM = 8;
	UINT IndicesM = 24;
	UINT Stride = sizeof(BBVertex);

	UINT VertexOffset = VerticesM;
	UINT IndexOffset = IndicesM;

	UINT offsetCnt = 0;
	UINT verticesCnt = 0;

	UINT propCnt = (UINT)m_propTypeVector.size() + 2;
	UINT vvSize = propCnt * VerticesM;
	UINT ivSize = IndicesM;
	std::vector<BBVertex> vertices(vvSize);
	std::vector<uint16_t> indices(ivSize);

	// 인덱스 값 주기
	uint16_t pnIndices[24];
	//ⓐ 윗면(Top) 사각형의 앞쪽 라인
	pnIndices[0] = 0; pnIndices[1] = 1;
	//ⓑ 윗면(Top) 사각형의 오른쪽 라인
	pnIndices[2] = 1; pnIndices[3] = 2;
	//ⓒ 윗면(Top) 사각형의 뒷쪽 라인
	pnIndices[4] = 2; pnIndices[5] = 3;
	//ⓓ 윗면(Top) 사각형의 왼쪽 라인
	pnIndices[6] = 3; pnIndices[7] = 0;
	//ⓔ 앞면(Front) 사각형의 왼쪽 라인
	pnIndices[8] = 0; pnIndices[9] = 4;
	//ⓕ 앞면(Front) 사각형의 오른쪽 라인
	pnIndices[10] = 1; pnIndices[11] = 5;
	//ⓖ 뒷면(Back) 사각형의 오른쪽 라인
	pnIndices[12] = 2; pnIndices[13] = 6;
	//ⓗ 뒷면(Back) 사각형의 왼쪽 라인
	pnIndices[14] = 3; pnIndices[15] = 7;
	//ⓘ 아랫면(Bottom) 사각형의 앞쪽 라인
	pnIndices[16] = 4; pnIndices[17] = 5;
	//ⓙ 아랫면(Bottom) 사각형의 오른쪽 라인
	pnIndices[18] = 5; pnIndices[19] = 6;
	//ⓚ 아랫면(Bottom) 사각형의 뒷쪽 라인
	pnIndices[20] = 6; pnIndices[21] = 7;
	//ⓛ 아랫면(Bottom) 사각형의 왼쪽 라인
	pnIndices[22] = 7; pnIndices[23] = 4;

	for (int j = 0; j < IndicesM; ++j)
		indices[j] = pnIndices[j];

	for (auto propType : m_propTypeVector)
	{
		XMFLOAT4 fColor(1.0f, 0.0f, 0.0f, 1.0f);
		if(propType == OBJECT_MESH_STR_ATTACK_BOX)
			fColor = XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f);

		float fx = m_PropBoundingBox[propType]->Extents.x * 0.5f;
		float fy = m_PropBoundingBox[propType]->Extents.y * 0.5f;
		float fz = m_PropBoundingBox[propType]->Extents.z * 0.5f;
		XMFLOAT3 c = m_PropBoundingBox[propType]->Center;
		c.y += fy;
		BBVertex pVertices[8];
		pVertices[0] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, -fy, -fz)), fColor);
		pVertices[1] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, -fy, -fz)), fColor);
		pVertices[2] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, -fy, +fz)), fColor);
		pVertices[3] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, -fy, +fz)), fColor);
		pVertices[4] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, +fy, -fz)), fColor);
		pVertices[5] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, +fy, -fz)), fColor);
		pVertices[6] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, +fy, +fz)), fColor);
		pVertices[7] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, +fy, +fz)), fColor);

		for (int i = 0; i < VerticesM; ++i, ++verticesCnt)
			vertices[verticesCnt] = pVertices[i];

		SubmeshGeometry bbSubmesh;
		bbSubmesh.IndexCount = IndicesM;
		bbSubmesh.StartIndexLocation = offsetCnt * IndexOffset;
		bbSubmesh.BaseVertexLocation = offsetCnt * VertexOffset;

		bb->DrawArgs[propType] = bbSubmesh;
		offsetCnt++;
	}

	// CharacterBoundingBox
	string CharacterType;
	for (int ki = 0; ki < 2; ++ki)
	{
		if (ki == 0)
			CharacterType = CHARACTER_COWBOY;
		else if (ki == 1)
			CharacterType = CHARACTER_GUNMAN;

		XMFLOAT4 fColor(1.0f, 0.0f, 0.0f, 1.0f);
		float fx = m_PropBoundingBox[CharacterType]->Extents.x * 0.5f;
		float fy = m_PropBoundingBox[CharacterType]->Extents.y * 0.5f;
		float fz = m_PropBoundingBox[CharacterType]->Extents.z * 0.5f;
		XMFLOAT3 c = m_PropBoundingBox[CharacterType]->Center;
		c.y += fy;
		BBVertex pVertices[8];
		pVertices[0] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, -fy, -fz)), fColor);
		pVertices[1] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, -fy, -fz)), fColor);
		pVertices[2] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, -fy, +fz)), fColor);
		pVertices[3] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, -fy, +fz)), fColor);
		pVertices[4] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, +fy, -fz)), fColor);
		pVertices[5] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, +fy, -fz)), fColor);
		pVertices[6] = BBVertex(MathHelper::Add(c, XMFLOAT3(+fx, +fy, +fz)), fColor);
		pVertices[7] = BBVertex(MathHelper::Add(c, XMFLOAT3(-fx, +fy, +fz)), fColor);

		for (int i = 0; i < VerticesM; ++i, ++verticesCnt)
			vertices[verticesCnt] = pVertices[i];

		SubmeshGeometry bbSubmesh;
		bbSubmesh.IndexCount = IndicesM;
		bbSubmesh.StartIndexLocation = offsetCnt * IndexOffset;
		bbSubmesh.BaseVertexLocation = offsetCnt * VertexOffset;

		bb->DrawArgs[CharacterType] = bbSubmesh;
		offsetCnt++;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(BBVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &bb->VertexBufferCPU));
	CopyMemory(bb->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &bb->IndexBufferCPU));
	CopyMemory(bb->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	bb->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, vertices.data(), vbByteSize, bb->VertexBufferUploader);

	bb->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, indices.data(), ibByteSize, bb->IndexBufferUploader);

	bb->VertexByteStride = sizeof(BBVertex);
	bb->VertexBufferByteSize = vbByteSize;
	bb->IndexFormat = DXGI_FORMAT_R16_UINT;
	bb->IndexBufferByteSize = ibByteSize;

	m_BoundingBoxMesh = std::move(bb);
}

void AssertsReference::BuildModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string meshName)
{
	// meshName으로 같은 메쉬가 있는지 체크
	if (m_GeometryMesh.count(meshName)) {
		cout << "이미 해당메쉬는 로드되었습니다." << endl;
	}
	else
	{
		string path = "Models\\Model\\" + meshName;

		std::vector<Vertex> vertices;
		std::vector<std::uint32_t> indices;
		std::vector<Material> materials;

		LoadMeshFile(vertices, indices, &materials, path);

		const UINT vbByteSize = vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = indices.size() * sizeof(std::int32_t);

		auto geo = std::make_unique<GeometryMesh>();
		geo->Name = meshName;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice, pCommandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice, pCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(Vertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R32_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		// Bounds
		BoundingBox bounds;

		auto iter = m_PropBoundingBox.find(meshName);
		if (iter != m_PropBoundingBox.end())
		{
			bounds = *(m_PropBoundingBox[meshName]);
		}
		else
		{
			XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
			XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
			XMVECTOR vMin = XMLoadFloat3(&vMinf3);
			XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

			for (auto& p : vertices)
			{
				XMVECTOR P = XMLoadFloat3(&p.Pos);

				vMin = XMVectorMin(vMin, P);
				vMax = XMVectorMax(vMax, P);
			}

			XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
			XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));
		}

		SubmeshGeometry submesh;
		submesh.IndexCount = indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;
		submesh.Bounds = bounds;

		geo->DrawArgs[meshName] = submesh;
		m_GeometryMesh[meshName] = std::move(geo);
	}
}

void AssertsReference::BuildSkinnedModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string meshName)
{
	if (m_GeometryMesh.count(meshName)) {
		cout << "이미 해당메쉬는 로드되었습니다." << endl;
	}
	else
	{
		string path = "Models\\SkinnedModel\\" + meshName + "\\" + meshName;

		std::vector<SkinnedVertex> skinnedVertices;
		std::vector<std::uint32_t> indices;
		std::vector<Material> materials;
		std::unique_ptr<SkinnedData> skinnedInfo = make_unique<SkinnedData>();

		LoadMeshFile(skinnedVertices, indices, &materials, path);
		LoadSkeletonFile(*skinnedInfo, path);

		// Geo CreateDefaultBuffer
		std::unique_ptr<SkinnedModelInstance> skinnedModelInst = std::make_unique<SkinnedModelInstance>();
		skinnedModelInst->SkinnedInfo = std::move(skinnedInfo);
		skinnedModelInst->FinalTransforms.resize(skinnedModelInst->SkinnedInfo->BoneCount());
		m_SkinnedModelInsts[meshName] = std::move(skinnedModelInst);

		const UINT vbByteSize = (UINT)skinnedVertices.size() * sizeof(SkinnedVertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

		auto geo = std::make_unique<GeometryMesh>();
		geo->Name = meshName;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), skinnedVertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
			pCommandList, skinnedVertices.data(), vbByteSize, geo->VertexBufferUploader);

		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
			pCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(SkinnedVertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R32_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		// Bounds
		XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
		XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
		XMVECTOR vMin = XMLoadFloat3(&vMinf3);
		XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

		for (auto& p : skinnedVertices)
		{
			XMVECTOR P = XMLoadFloat3(&p.Pos);

			vMin = XMVectorMin(vMin, P);
			vMax = XMVectorMax(vMax, P);
		}

		BoundingBox bounds;
		XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
		XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));

		SubmeshGeometry submesh;
		submesh.IndexCount = indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;
		submesh.Bounds = bounds;
		geo->DrawArgs[meshName] = submesh;

		m_GeometryMesh[meshName] = std::move(geo);
	}
}

void AssertsReference::BuildSkinnedModelAnimation(std::string meshName, const std::string clipName)
{
	if (!m_SkinnedModelInsts.count(meshName))
	{
		cout << "LoadModelAnimation: None MeshName" << endl;
	}
	else
	{
		string path = "Models\\SkinnedModel\\" + meshName + "\\" + meshName;
		LoadAnimationFile(*m_SkinnedModelInsts[meshName]->SkinnedInfo, path, clipName);
	}
}

void AssertsReference::BuildSkinnedModelSubMesh(std::string meshName, const std::string submeshName)
{
	if (!m_SkinnedModelInsts.count(meshName))
	{
		cout << "BuildSkinnedModelSubMesh: None MeshName" << endl;
	}
	else
	{
		auto vSubmeshOffset = m_SkinnedModelInsts[meshName]->SkinnedInfo->GetSubmeshOffset();
		auto vBoneName = m_SkinnedModelInsts[meshName]->SkinnedInfo->GetBoneName();

		// submeshName의 객체가 있는지 예외처리
		bool checkIsSubmeshName = false;
		for (auto& boneName : vBoneName)
		{
			if (boneName == submeshName) checkIsSubmeshName = true;
		}

		// submesh 추가
		if (!checkIsSubmeshName)
		{
			cout << "BuildSkinnedModelSubMesh: None submeshName" << endl;
		}
		else
		{
			// caculate submesh offset
			UINT indexCount = 0;
			UINT startIndexLocation = 0;
			for (int i = 0; i < vBoneName.size(); ++i)
			{
				startIndexLocation = vSubmeshOffset[i];
				indexCount += startIndexLocation;

				if (vBoneName[i] == submeshName) break;
			}

			SubmeshGeometry submesh;
			submesh.IndexCount = indexCount;
			submesh.StartIndexLocation = startIndexLocation;
			submesh.BaseVertexLocation = 0;

			m_GeometryMesh[meshName]->DrawArgs[submeshName] = submesh;
		}
	}
}

void AssertsReference::BuildBasicParticle(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string particleName, int particleCount, DirectX::XMFLOAT2 particlePosX, DirectX::XMFLOAT2 particlePosY, DirectX::XMFLOAT2 particlePosZ, DirectX::XMFLOAT2 particleSize, DirectX::XMFLOAT2 particleVelX, DirectX::XMFLOAT2 particleVelY, DirectX::XMFLOAT2 particleVelZ, DirectX::XMFLOAT2 particleStartTime, DirectX::XMFLOAT2 particleLifeTime, DirectX::XMFLOAT2 particlePeriod, DirectX::XMFLOAT2 particleAmplifier)
{
	// 입력받아야 할 자료들
	// world, particle name
	// particle Param: pos, size,,,, 
	// build particle verticies
	std::vector<ParticleVertex> particleVertices;
	particleVertices.resize(particleCount);

	for (int i = 0; i < particleCount; ++i)
	{
		particleVertices[i].pos = XMFLOAT3(MathHelper::RandF(particlePosX.x, particlePosX.y), MathHelper::RandF(particlePosY.x, particlePosY.y), MathHelper::RandF(particlePosZ.x, particlePosZ.y));;
		particleVertices[i].size = particleSize;
		particleVertices[i].velocity = XMFLOAT3(MathHelper::RandF(particleVelX.x, particleVelX.y), MathHelper::RandF(particleVelY.x, particleVelY.y), MathHelper::RandF(particleVelZ.x, particleVelZ.y));
		particleVertices[i].startTime = MathHelper::RandF(particleStartTime.x, particleStartTime.y);
		particleVertices[i].lifeTime = MathHelper::RandF(particleLifeTime.x, particleLifeTime.y);
		particleVertices[i].period = MathHelper::RandF(particlePeriod.x, particlePeriod.y);
		particleVertices[i].amplifier = MathHelper::RandF(particleAmplifier.x, particleAmplifier.y);
	}

	// build paritcle indicies
	std::vector<std::uint16_t> indices;
	indices.resize(particleCount);

	for (int i = 0; i < particleCount; ++i)
	{
		indices[i] = i;
	}

	AllocateParticleBuffer(pDevice, pCommandList, particleName, particleVertices, indices);
}

void AssertsReference::BuildCircleParticle(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string particleName, int particleCount, DirectX::XMFLOAT3 particlePos, DirectX::XMFLOAT2 particleSize, DirectX::XMFLOAT2 particleVelX, DirectX::XMFLOAT2 particleVelY, DirectX::XMFLOAT2 particleVelZ, DirectX::XMFLOAT2 particleStartTime, DirectX::XMFLOAT2 particleLifeTime, DirectX::XMFLOAT2 particlePeriod, DirectX::XMFLOAT2 particleAmplifier, float radius)
{
	// 입력받아야 할 자료들
	// world, particle name
	// particle Param: pos, size,,,, 
	// build particle verticies
	std::vector<ParticleVertex> particleVertices;
	particleVertices.resize(particleCount);

	for (int i = 0; i < particleCount; ++i)
	{
		float randomRadius = MathHelper::RandF(0, 1);
		XMFLOAT3 initParametricPos = XMFLOAT3(radius * sin(randomRadius * 2.0 * XM_PI), 0, radius * cos(randomRadius * 2.0 * XM_PI));
		XMFLOAT3 newPos = MathHelper::Add(particlePos, initParametricPos);

		particleVertices[i].pos = newPos;
		particleVertices[i].size = particleSize;
		particleVertices[i].velocity = XMFLOAT3(MathHelper::RandF(particleVelX.x, particleVelX.y), MathHelper::RandF(particleVelY.x, particleVelY.y), MathHelper::RandF(particleVelZ.x, particleVelZ.y));
		particleVertices[i].startTime = MathHelper::RandF(particleStartTime.x, particleStartTime.y);
		particleVertices[i].lifeTime = MathHelper::RandF(particleLifeTime.x, particleLifeTime.y);
		particleVertices[i].period = MathHelper::RandF(particlePeriod.x, particlePeriod.y);
		particleVertices[i].amplifier = MathHelper::RandF(particleAmplifier.x, particleAmplifier.y);
	}

	///////////// 파티클 메시제작 동일
	// build paritcle indicies
	std::vector<std::uint16_t> indices;
	indices.resize(particleCount);

	for (int i = 0; i < particleCount; ++i)
	{
		indices[i] = i;
	}

	AllocateParticleBuffer(pDevice, pCommandList, particleName, particleVertices, indices);
}

bool AssertsReference::LoadMeshFile(std::vector<Vertex>& outVertexVector, std::vector<uint32_t>& outIndexVector, std::vector<Material>* outMaterial, std::string path)
{
	path += ".mesh";
	std::ifstream fileIn(path);

	uint32_t vertexSize, indexSize;
	uint32_t materialSize;

	std::string ignore;
	if (fileIn)
	{
		fileIn >> ignore >> vertexSize;
		fileIn >> ignore >> indexSize;
		fileIn >> ignore >> materialSize;

		if (vertexSize == 0 || indexSize == 0)
			return false;

		// Material Data
		fileIn >> ignore;
		for (uint32_t i = 0; i < materialSize; ++i)
		{
			Material tempMaterial;

			fileIn >> ignore >> tempMaterial.Name;
			fileIn >> ignore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
			fileIn >> ignore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
			fileIn >> ignore >> tempMaterial.FresnelR0.x >> tempMaterial.FresnelR0.y >> tempMaterial.FresnelR0.z;
			fileIn >> ignore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
			fileIn >> ignore >> tempMaterial.Emissive.x >> tempMaterial.Emissive.y >> tempMaterial.Emissive.z;
			fileIn >> ignore >> tempMaterial.Roughness;
			fileIn >> ignore;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					fileIn >> tempMaterial.MatTransform.m[i][j];
				}
			}
			(*outMaterial).push_back(tempMaterial);
		}

		// Vertex Data
		for (uint32_t i = 0; i < vertexSize; ++i)
		{
			Vertex vertex;
			fileIn >> ignore >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
			fileIn >> ignore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
			fileIn >> ignore >> vertex.TexC.x >> vertex.TexC.y;
			fileIn >> ignore >> vertex.Tangent.x >> vertex.Tangent.y >> vertex.Tangent.z;
			fileIn >> ignore >> vertex.Binormal.x >> vertex.Binormal.y >> vertex.Binormal.z;

			// push_back
			outVertexVector.push_back(vertex);
		}

		// Index Data
		fileIn >> ignore;
		for (uint32_t i = 0; i < indexSize; ++i)
		{
			uint32_t index;
			fileIn >> index;
			outIndexVector.push_back(index);
		}

		return true;
	}

	return false;
}

bool AssertsReference::LoadMeshFile(std::vector<SkinnedVertex>& outVertexVector, std::vector<uint32_t>& outIndexVector, std::vector<Material>* outMaterial, std::string path)
{
	path += ".mesh";
	std::ifstream fileIn(path);

	uint32_t vertexSize, indexSize;
	uint32_t materialSize;

	std::string ignore;
	if (fileIn)
	{
		fileIn >> ignore >> vertexSize;
		fileIn >> ignore >> indexSize;
		fileIn >> ignore >> materialSize;

		if (vertexSize == 0 || indexSize == 0)
			return false;

		if (outMaterial != nullptr)
		{
			// Material Data
			fileIn >> ignore;
			for (uint32_t i = 0; i < materialSize; ++i)
			{
				Material tempMaterial;

				fileIn >> ignore >> tempMaterial.Name;
				fileIn >> ignore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
				fileIn >> ignore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
				fileIn >> ignore >> tempMaterial.FresnelR0.x >> tempMaterial.FresnelR0.y >> tempMaterial.FresnelR0.z;
				fileIn >> ignore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
				fileIn >> ignore >> tempMaterial.Emissive.x >> tempMaterial.Emissive.y >> tempMaterial.Emissive.z;
				fileIn >> ignore >> tempMaterial.Roughness;
				fileIn >> ignore;
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						fileIn >> tempMaterial.MatTransform.m[i][j];
					}
				}
				(*outMaterial).push_back(tempMaterial);
			}
		}

		// Vertex Data
		for (uint32_t i = 0; i < vertexSize; ++i)
		{
			SkinnedVertex vertex;
			int temp[4];
			fileIn >> ignore >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
			fileIn >> ignore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
			fileIn >> ignore >> vertex.TexC.x >> vertex.TexC.y;
			fileIn >> ignore >> vertex.Tangent.x >> vertex.Tangent.y >> vertex.Tangent.z;
			fileIn >> ignore >> vertex.Binormal.x >> vertex.Binormal.y >> vertex.Binormal.z;
			fileIn >> ignore >> vertex.BoneWeights.x >> vertex.BoneWeights.y >> vertex.BoneWeights.z;
			fileIn >> ignore >> temp[0] >> temp[1] >> temp[2] >> temp[3];

			for (int j = 0; j < 4; ++j)
			{
				vertex.BoneIndices[j] = temp[j];
			}
			// push_back
			outVertexVector.push_back(vertex);
		}

		// Index Data
		fileIn >> ignore;
		for (uint32_t i = 0; i < indexSize; ++i)
		{
			uint32_t index;
			fileIn >> index;
			outIndexVector.push_back(index);
		}

		return true;
	}

	return false;
}

bool AssertsReference::LoadSkeletonFile(SkinnedData& outSkinnedData, std::string path)
{
	path += ".skeleton";
	std::ifstream fileIn(path);

	uint32_t boneSize;

	std::string ignore;
	if (fileIn)
	{
		fileIn >> ignore >> boneSize;

		if (boneSize == 0)
			return false;

		// Bone Data
		// Bone Hierarchy
		fileIn >> ignore;
		std::vector<int> boneHierarchy;
		for (uint32_t i = 0; i < boneSize; ++i)
		{
			int tempBoneHierarchy;
			fileIn >> tempBoneHierarchy;
			boneHierarchy.push_back(tempBoneHierarchy);
		}

		fileIn >> ignore;
		for (uint32_t i = 0; i < boneSize; ++i)
		{
			std::string tempBoneName;
			fileIn >> tempBoneName;

			outSkinnedData.SetBoneName(tempBoneName);
		}
		// Bone Offset
		fileIn >> ignore;
		std::vector<DirectX::XMFLOAT4X4> boneOffsets;
		for (uint32_t i = 0; i < boneSize; ++i)
		{
			DirectX::XMFLOAT4X4 tempBoneOffset;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					fileIn >> tempBoneOffset.m[i][j];
				}
			}
			boneOffsets.push_back(tempBoneOffset);
		}
		// Bone Submesh Offset
		fileIn >> ignore;
		std::vector<int> boneSubmeshOffset;
		for (uint32_t i = 0; i < boneSize; ++i)
		{
			int tempBoneSubmeshOffset;
			fileIn >> tempBoneSubmeshOffset;
			outSkinnedData.SetSubmeshOffset(tempBoneSubmeshOffset);
		}

		outSkinnedData.Set(
			boneHierarchy,
			boneOffsets);

		return true;
	}

	return false;
}

bool AssertsReference::LoadAnimationFile(SkinnedData& outSkinnedData, std::string& path, const std::string clipName)
{
	path = path + "_" + clipName + ".anim";
	std::ifstream fileIn(path);

	AnimationClip animation;
	uint32_t boneAnimationSize, keyframeSize;

	std::string ignore;
	if (fileIn)
	{
		fileIn >> ignore >> boneAnimationSize;
		fileIn >> ignore >> keyframeSize;

		for (uint32_t i = 0; i < boneAnimationSize; ++i)
		{
			BoneAnimation boneAnim;
			for (uint32_t j = 0; j < keyframeSize; ++j)
			{
				Keyframe key;
				fileIn >> key.TimePos;
				fileIn >> key.Translation.x >> key.Translation.y >> key.Translation.z;
				fileIn >> key.Scale.x >> key.Scale.y >> key.Scale.z;
				fileIn >> key.RotationQuat.x >> key.RotationQuat.y >> key.RotationQuat.z >> key.RotationQuat.w;
				boneAnim.Keyframes.push_back(key);
			}
			animation.BoneAnimations.push_back(boneAnim);
		}

		outSkinnedData.SetAnimation(animation, clipName);
		return true;
	}

	return false;
}

void AssertsReference::AllocateParticleBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string particleName, std::vector<ParticleVertex>& particleVertices, std::vector< uint16_t>& indices)
{
	const UINT vbByteSize = (UINT)particleVertices.size() * sizeof(ParticleVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<GeometryMesh>();
	geo->Name = particleName;

	// 파티클 메시(정점, 인덱스 버퍼) 리소스 할당
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), particleVertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, particleVertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(ParticleVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs[particleName] = submesh;

	m_GeometryMesh[particleName] = std::move(geo);

	// 멤버변수 메모리 해제
	particleVertices.clear();
	indices.clear();
}
