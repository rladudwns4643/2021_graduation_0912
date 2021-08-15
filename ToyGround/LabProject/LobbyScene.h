#pragma once
#include "Scene.h"

#define CLIENT_EVENT_MATCHINGROOM_UI_PRESSED 0xFFFF10
#define CLIENT_EVENT_MATCHINGROOM_UI_INPUT_PRESSED 0xFFFF11

namespace Graphics
{
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
	extern Microsoft::WRL::ComPtr<ID3D12PipelineState> g_UIPSO;
}

enum CHATTYPE
{
	NONE, ID, PASSWORD
};

namespace InputHandler
{
	extern bool g_CursorSwitch;
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
	virtual void RenderText() override;

private:
	std::string _id;
	std::string _password;

public:
	std::wstring m_ID;
	std::wstring m_Password;

	int m_ChatType;

	bool ID_Input_Ativate = false;
	bool PW_Input_Ativate = false;

	XMFLOAT2 m_ScaleConvert{ 1.f, 1.f };

	bool m_isMatching = false;

	float LoadingBarPosX = -253;
	int LoadingBarDirection = 1;
	float LoadingBarSpeed = 0;
	float LoadingBarAccel = 0.2;
	int LoadingTest = 0;
};

