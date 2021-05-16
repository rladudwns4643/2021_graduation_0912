#pragma once
#include "Scene.h"

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_AABBoxPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OBBoxPSO;
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

private:
	void ChangeFreeCamera();

public:
	bool m_PauseScene = true;

private:
	// 플레이어들
	std::map<int, Character*> m_Users;
	int m_PlayerID;

	std::vector<int> m_player_in_room;

	// 기타 이벤트 변수들
	std::string m_MapName;
	int			m_Timer;

private:
	bool m_IsGameOver = false;
	int m_GemId = 0;
};

