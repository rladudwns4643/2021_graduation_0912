#pragma once
#include "Singleton.h"
#include "UploadBuffer.h"

class SkinnedModelInstance;
class GameObject;
class BBox;
class ObjectInfo;
class Camera;
class GameTimer;

namespace Core
{
	extern GameTimer* g_GameTimer;

	extern Microsoft::WRL::ComPtr<ID3D12Device> g_Device;
	extern Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_CommandList;

	extern Microsoft::WRL::ComPtr<ID2D1DeviceContext2> g_D2dDeviceContext;
	extern Microsoft::WRL::ComPtr<IDWriteFactory> g_DWriteFactory;

	extern int g_DisplayWidth;
	extern int g_DisplayHeight;
}

class CommandContext abstract
{
public:
	virtual void Release() = 0;

protected:
	ID3D12RootSignature* m_CurGraphicsRootSignature = NULL;
	ID3D12PipelineState* m_CurPipelineState = NULL;
	ID3D12RootSignature* m_CurComputeRootSignature = NULL;
};

struct UIInfo
{
	XMFLOAT2 pos;
	XMFLOAT2 size;
};

class GraphicsContext : public CommandContext, public TemplateSingleton<GraphicsContext>
{
public:
	void Initialize();
	virtual void Release() override;

public:
	void BuildInstanceBuffer(ObjectInfo* objInfo);

	void Update2DPosition(ObjectInfo* objInfo, std::vector<GameObject*>& rItems);
	void UpdateInstanceData(ObjectInfo* objInfo, std::vector<GameObject*>& rItems, bool isFrustum = true);
	void UpdateInstanceDatas(std::vector<ObjectInfo*>& objInfos, std::vector<GameObject*>& rItems); // 사용안함

	void UpdateMaterialBuffer(std::unordered_map<std::string, std::unique_ptr<Material>>& materials);
	void UpdateMainPassCB(Camera& camera, Light* light);

	void UpdateSkinnedCBs(UINT skinnedCBIndex, SkinnedModelInstance* skinmodelInstance);

	void DrawRenderItem(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, int zLayer = -1, bool isFrustum = true);
	void DrawBoundingBox(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, bool drawAABB = true, int zLayer = -1, bool isFrustum = true);
	void DrawRenderItems(std::vector<ObjectInfo*>& objInfos, const std::vector<GameObject*>& rItems);

	void DrawBBox(const std::vector<BBox*>& rItems);

public:
	void SetPipelineState(ID3D12PipelineState* PSO);
	void SetGraphicsRootSignature(ID3D12RootSignature* RootSignature);

	ID3D12PipelineState* GetPipelineState() { return m_CurPipelineState; };

public:
	ShaderResource::PassConstants mMainPassCB;

	std::unique_ptr<UploadBuffer<ShaderResource::PassConstants>>	PassCB = nullptr;
	std::map<string, std::unique_ptr<UploadBuffer<ShaderResource::InstanceData>>> m_InstanceBuffers;
	std::array< std::unique_ptr<UploadBuffer<ShaderResource::SkinnedConstants>>, BoneIndex::Count> m_SkinnedCBs;
	std::unique_ptr<UploadBuffer<ShaderResource::MaterialData>>		MaterialBuffer = nullptr;

	UINT passCount; UINT materialCount; UINT skinnedObjectCount;

private:
	//DirectX::BoundingSphere mSceneBounds;

	float mLightNearZ = 0.0f;
	float mLightFarZ = 0.0f;
	XMFLOAT3 mLightPosW;
	XMFLOAT4X4 mLightView = MathHelper::Identity4x4();
	XMFLOAT4X4 mLightProj = MathHelper::Identity4x4();

	float mLightRotationAngle = 0.0f;
	XMFLOAT3 mRotatedLightDirections;

};