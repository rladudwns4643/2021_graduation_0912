#pragma once
#include "Scene.h"

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkinnedPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkyPSO;
}

namespace InputHandler
{
	extern bool g_CursorSwitch;
}

class Character;

class GameplayScene : public Scene
{
	friend GameplayController;

public:
	virtual void Initialize() override;
	virtual void OnResize() override;

public:
	virtual bool Enter() override;
	virtual void Exit() override;

	virtual void Update(const float& fDeltaTime) override;
	virtual void Render() override;

private:
	void ChangeFreeCamera();

private:
	// 플레이어들
	std::map<int, Character*> m_Users;
	int m_PlayerID;

	// 기타 이벤트 변수들
	std::string m_MapName;
	int			m_Timer;

private:
	bool m_IsGameOver = false;
};

