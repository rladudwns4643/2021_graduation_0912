#pragma once
#include "Scene.h"


namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
}

class LobbyScene : public Scene
{
	friend LobbyController;
private:
	virtual void ProcessEvent(int sEvent, int argsCount = 0, ...) override;
public:
	virtual void Initialize() override;
	virtual void OnResize() override;

public:
	virtual bool Enter() override;
	virtual void Exit() override;

	virtual void Update(const float& fDeltaTime) override;
	virtual void Render() override;
	virtual void RenderUI() override;
};

