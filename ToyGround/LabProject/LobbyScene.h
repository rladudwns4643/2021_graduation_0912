#pragma once
#include "Scene.h"


namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
}

class LobbyScene : public Scene
{
	friend LobbyController;

public:
	virtual void Initialize() override;
	virtual void OnResize() override;

public:
	virtual bool Enter() override;
	virtual void Exit() override;

	virtual void Update(const float& fDeltaTime) override;
	virtual void Render() override;
};

