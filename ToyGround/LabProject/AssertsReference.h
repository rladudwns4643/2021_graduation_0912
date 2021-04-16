#pragma once
#include "Singleton.h"
#include "GeometryMesh.h"

// ���� ������ �޽��� �� �ѹ��� �����Ǿ��Ѵ�.
// �޽��� �����͸� �����־ ������Ʈ���� �����͸� ������ ����.
// �޽� ������ ������ �����Ѵ�.
class Map;
class AssertsReference final : public TemplateSingleton<AssertsReference>
{
public:
	Map* LoadMapInfo(string mapName);
	void CreateBB();
	void BuildMaterials();

	void BuildGeoMeshes(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void BuildModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string meshName);

private:
	bool LoadMeshFile(
		std::vector<Vertex>& outVertexVector,
		std::vector<uint32_t>& outIndexVector,
		std::vector<Material>* outMaterial,
		std::string path);

public:
	std::unordered_map<std::string, std::unique_ptr<GeometryMesh>>	m_GeometryMesh;
	std::unordered_map<std::string, std::unique_ptr<Material>>	m_Materials;
	std::map<std::string, std::unique_ptr<BoundingBox>> m_PropBoundingBox;
	int mapArray[MAP_HEIGHT_BLOCK_NUM][MAP_DEPTH_BLOCK_NUM][MAP_WIDTH_BLOCK_NUM];
};

