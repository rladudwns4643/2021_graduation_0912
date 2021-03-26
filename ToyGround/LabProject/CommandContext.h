#pragma once
#include "Singleton.h"
#include "UploadBuffer.h"

class GameObject;
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
	ID3D12RootSignature* m_CurGraphicsRootSignature;
	ID3D12PipelineState* m_CurPipelineState;
	ID3D12RootSignature* m_CurComputeRootSignature;
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

	void UpdateInstanceData(ObjectInfo* objInfo, std::vector<GameObject*>& rItems, bool isFrustum = false);
	void UpdateInstanceDatas(std::vector<ObjectInfo*>& objInfos, std::vector<GameObject*>& rItems); // ������

	void UpdateMaterialBuffer(std::unordered_map<std::string, std::unique_ptr<Material>>& materials);
	void UpdateMainPassCB(Camera& camera, Light* light);

	void DrawRenderItem(ObjectInfo* objInfo, const std::vector<GameObject*>& rItems, int zLayer = -1, bool isFrustum = false);
	void DrawRenderItems(std::vector<ObjectInfo*>& objInfos, const std::vector<GameObject*>& rItems);
	
public:
	void SetPipelineState(ID3D12PipelineState* PSO);
	void SetGraphicsRootSignature(ID3D12RootSignature* RootSignature);

	ID3D12PipelineState* GetPipelineState() { return m_CurPipelineState; };

public:
	ShaderResource::PassConstants mMainPassCB;

	std::unique_ptr<UploadBuffer<ShaderResource::PassConstants>>	PassCB = nullptr;
	std::map<string, std::unique_ptr<UploadBuffer<ShaderResource::InstanceData>>> m_InstanceBuffers;
	std::unique_ptr<UploadBuffer<ShaderResource::MaterialData>>		MaterialBuffer = nullptr;

	UINT passCount; UINT materialCount;

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