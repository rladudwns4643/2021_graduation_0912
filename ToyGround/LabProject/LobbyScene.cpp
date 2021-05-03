#include "pch.h"
#include "LobbyScene.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "Timer.h"
#include "InputHandler.h"

void LobbyScene::Initialize()
{
	// Scene-Controller
	m_SceneController = new LobbyController(this);
}

void LobbyScene::OnResize()
{
	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eLobby);
}

bool LobbyScene::Enter()
{
	cout << "============= Lobby Scene ==============" << endl;

	return false;
}

void LobbyScene::Exit()
{
}

void LobbyScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);
}

void LobbyScene::Render()
{
	// Main rendering pass
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());
}
