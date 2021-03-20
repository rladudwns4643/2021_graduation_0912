#pragma once
#include "Scene.h"

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
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
	int			m_Timer;
	short		m_PenaltyDuration;

private:
	// 쓰레드 이벤트동기화 변수들
	bool m_IsGameOver = false;
};

