#pragma once
#include "Singleton.h"
#include "GeometryMesh.h"
#include "BoundingBoxMesh.h"
#include "SkinnedModelInstance.h"

// 같은 종류의 메쉬는 딱 한번만 생성되야한다.
// 메쉬들 포인터만 갖고있어서 오브젝트에서 포인터만 가져다 쓴다.
// 메쉬 제작은 씬에서 제작한다.
class Map;
class AssertsReference final : public TemplateSingleton<AssertsReference>
{
public:
	Map* LoadMapInfo(string mapName);
	void CreateBB();
	void BuildMaterials();

	void BuildGeoMeshes(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void BuildBoundingBoxMeshes(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void BuildModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string meshName);

	void BuildSkinnedModel(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string meshName);
	void BuildSkinnedModelAnimation(std::string meshName, const std::string clipName);
	void BuildSkinnedModelSubMesh(std::string meshName, const std::string submeshName);

	void BuildBasicParticle(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList,
		std::string particleName, int particleCount,
		DirectX::XMFLOAT2 particlePosX, DirectX::XMFLOAT2 particlePosY, DirectX::XMFLOAT2 particlePosZ,
		DirectX::XMFLOAT2 particleSize,
		DirectX::XMFLOAT2 particleVelX, DirectX::XMFLOAT2 particleVelY, DirectX::XMFLOAT2 particleVelZ,
		DirectX::XMFLOAT2 particleStartTime, DirectX::XMFLOAT2 particleLifeTime,
		DirectX::XMFLOAT2 particlePeriod, DirectX::XMFLOAT2 particleAmplifier);

	void BuildCircleParticle(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList,
		std::string particleName, int particleCount,
		DirectX::XMFLOAT3 particlePos, DirectX::XMFLOAT2 particleSize,
		DirectX::XMFLOAT2 particleVelX, DirectX::XMFLOAT2 particleVelY, DirectX::XMFLOAT2 particleVelZ,
		DirectX::XMFLOAT2 particleStartTime, DirectX::XMFLOAT2 particleLifeTime,
		DirectX::XMFLOAT2 particlePeriod, DirectX::XMFLOAT2 particleAmplifier,
		float radius);

private:
	bool LoadMeshFile(
		std::vector<Vertex>& outVertexVector,
		std::vector<uint32_t>& outIndexVector,
		std::vector<Material>* outMaterial,
		std::string path);
	bool LoadMeshFile(
		std::vector<SkinnedVertex>& outVertexVector,
		std::vector<uint32_t>& outIndexVector,
		std::vector<Material>* outMaterial,
		std::string path);

	bool LoadSkeletonFile(SkinnedData& outSkinnedData, std::string path);
	bool LoadAnimationFile(SkinnedData& outSkinnedData, std::string& path, const std::string clipName);

	void AllocateParticleBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, std::string particleName, std::vector<ParticleVertex>& particleVertices, std::vector< uint16_t>& indices);

public:
	std::unordered_map<std::string, std::unique_ptr<GeometryMesh>>	m_GeometryMesh;
	std::unique_ptr<BoundingBoxMesh>								m_BoundingBoxMesh;
	std::map<std::string, std::unique_ptr<SkinnedModelInstance>> m_SkinnedModelInsts;
	std::unordered_map<std::string, std::unique_ptr<Material>>	m_Materials;
	std::map<std::string, std::unique_ptr<BoundingBox>> m_PropBoundingBox;

	std::vector<std::string> m_propTypeVector;
};

