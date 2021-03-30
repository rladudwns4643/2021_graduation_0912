#include "pch.h"
#include "AssertsReference.h"
#include "GeometryGenerator.h"
#include "Map.h"

Map* AssertsReference::LoadMapInfo()
{
	Map* map = new Map;
	set<string> propTypeSet;

	//MapTool::PlayerInfo prePlayerInfo;
	//prePlayerInfo.playerName = "FirstPlayer";
	//prePlayerInfo.position.x = 0.f;
	//prePlayerInfo.position.y = 0.f;
	//prePlayerInfo.position.z = 0.f;
	//prePlayerInfo.rotY = 0.f;
	//prePlayerInfo.spawnPos = 1;
	//map->playerInfoVector.emplace_back(prePlayerInfo);

	MapTool::MapInfo preInfo;

	float tmp;
	preInfo.meshName = OBJECT_MESH_STR_CUBE_01;
	preInfo.position.x = 0.f;
	preInfo.position.y = 0.f;
	preInfo.position.z = 0.f;
	preInfo.rotation.x = 0.f;
	preInfo.rotation.y = 0.f;
	preInfo.rotation.z = 0.f;
	preInfo.typeID = 1;
	preInfo.textureName = TEXTURE_STR_Cartoon_CubeWorld_Texture;

	map->mapInfoVector.emplace_back(preInfo);
	propTypeSet.insert(OBJECT_MESH_STR_CUBE_01);

	// propTypeVector: 중복되는 것 없이 에셋이름만 모아놓은 vector
	for (auto iter = propTypeSet.begin(); iter != propTypeSet.end(); ++iter)
	{
		map->propTypeVector.emplace_back(*iter);
	}

	return map;
}

void AssertsReference::BuildMaterials()
{
	auto sky = std::make_unique<Material>();
	sky->MatCBIndex = TEXTURE_INDEX_grasscube1024;
	sky->DiffuseSrvHeapIndex = TEXTURE_INDEX_grasscube1024;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0;

	auto Cartoon_CubeWorld_Texture  = std::make_unique<Material>();
	Cartoon_CubeWorld_Texture->MatCBIndex = TEXTURE_INDEX_Cartoon_CubeWorld_Texture;
	Cartoon_CubeWorld_Texture->DiffuseSrvHeapIndex = TEXTURE_INDEX_Cartoon_CubeWorld_Texture;
	Cartoon_CubeWorld_Texture->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Cartoon_CubeWorld_Texture->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	Cartoon_CubeWorld_Texture->Roughness = 0.967734f;

	m_Materials["grasscube1024"] = std::move(sky);
	m_Materials[TEXTURE_STR_Cartoon_CubeWorld_Texture] = std::move(Cartoon_CubeWorld_Texture);
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
		//BoundingBox bounds;
		//
		//auto iter = m_PropBoundingBox.find(meshName);
		//if (iter != m_PropBoundingBox.end())
		//{
		//	bounds = *(m_PropBoundingBox[meshName]);
		//}
		//else
		//{
		//	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
		//	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
		//	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
		//	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
		//
		//	for (auto& p : vertices)
		//	{
		//		XMVECTOR P = XMLoadFloat3(&p.Pos);
		//
		//		vMin = XMVectorMin(vMin, P);
		//		vMax = XMVectorMax(vMax, P);
		//	}
		//
		//	XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
		//	XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));
		//}

		SubmeshGeometry submesh;
		submesh.IndexCount = indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;
		//submesh.Bounds = bounds;

		geo->DrawArgs[meshName] = submesh;
		m_GeometryMesh[meshName] = std::move(geo);
	}
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
