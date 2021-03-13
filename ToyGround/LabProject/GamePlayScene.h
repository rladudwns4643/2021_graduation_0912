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
	virtual void WriteShadow() override;

	void ThreadEventHandler();

private:
	std::string ConvertCharacterNameToPlayboardImageName(std::string chrName);
	void SetUI2DProfilePicture(std::string ui2dName, std::string ui2dInstId, std::string playerName);
	void ChangeCameraNextPlayer();
	void ChangeFreeCamera();
	void DiePlayer();

private:

	/* 기타 이벤트 변수들 */
	int			m_Timer;
	short		m_PenaltyDuration;

	/* 내 캐릭터가 살아있는지 확인 */
	// 내 캐릭터가 죽었을시 씬컨트롤러 활성화를 위한 변수
	bool		m_IsArriveMyPlayer = true;
	int			m_IndexChangeOtherPlayerCamera = 0;

private:
	/* 쓰레드 이벤트동기화 변수들 */
	bool m_IsGameOver = false;
};

