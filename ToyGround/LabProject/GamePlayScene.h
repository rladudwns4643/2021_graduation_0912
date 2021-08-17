#pragma once
#include "Scene.h"

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkinnedPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_AABBoxPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OBBoxPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkyPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_UIPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_HPPSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_ShadowOpaquePSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkinnedShadowOpaquePSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_ParticlePSO;
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
	virtual void RenderText() override;
	virtual void WriteShadow() override;

	void CountDownReset()
	{
		m_isCountDownOn = false;
		m_countDown = 15;
		m_countDownID = 1;
		//SoundManager::GetApp()->StopBGM();
		//SoundManager::GetApp()->PlayBGM(L"MainBGM.mp3", 1.0f);
	}

public:
	bool m_PauseScene = true;

	bool m_isCountDownOn = false;
	int m_countDown;
	int m_countDownID;

	int m_CowBoyGemNum = 0;
	int m_GunManGemNum = 0;
	bool m_IsGameOver = false;

private:
	// 플레이어들
	std::map<int, Character*> m_Users;
	int m_PlayerID;
	string playername;
	string enemyname;

	std::vector<int> m_player_in_room;

	// 기타 이벤트 변수들
	std::string m_MapName;
	int			m_Timer{};

private:
	int m_Winner{ 1 };

	int m_StartCount = 4;
};

