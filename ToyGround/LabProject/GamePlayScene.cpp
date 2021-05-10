#include "pch.h"
#include "GameplayScene.h"
#include "TOY_GROUND.h"
#include "Timer.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "ApplicationContext.h"
#include "InputHandler.h"
#include "Service.h"

#include "GameObject.h"
#include "Character.h"
#include "Map.h"

void GameplayScene::ProcessEvent(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	case EVENT_LOBBY_LOGIN_REQUEST: {
		//login을 위한 id /pw 입력하는 곳, 지금은 dummylogin으로 넘어감
		break;
	}
	case EVENT_LOBBY_LOGIN_FAIL: {
		break;
	}
	case EVENT_LOBBY_SIGNUP_OK: {
		break;
	}
	case EVENT_LOBBY_SIGNUP_FAIL: {
		break;
	}
	case EVENT_LOBBY_UPDATE_CLIENT_USERINFO: {
		break;
	}
	case EVENT_LOBBY_MATCH_START: {
		break;
	}
	case EVENT_LOBBY_MATCH_CANCEL: {
		break;
	}
	}
}

void GameplayScene::Initialize()
{
	// Scene-Controller
	m_SceneController = new GameplayController(this);
	m_SceneController->SetMapName(MAP_STR_GAME_MAP);

	/* SkyCube */
	AppContext->CreateSkycube("gameplaySky", "gameplaySky", "SkyBox");

	/* 맵의 오브젝트들 생성 */
	AppContext->CreateProps(MAP_STR_GAME_MAP);
}

void GameplayScene::OnResize()
{
}

bool GameplayScene::Enter()
{
	cout << "============= Gameplay Scene ==============" << endl;

	// 게임플레이 씬에서 마우스 보이게 하고싶다면 주석처리
	InputHandler::g_CursorSwitch = false;

	/* Create SceneBounds for Shadow */
	m_SceneBounds.Center = XMFLOAT3(2500.f, 0.0f, 2500.f);
	m_SceneBounds.Radius = 2700.f;

	/* Light Setting */
	TOY_GROUND::GetApp()->m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 0.57735f, -0.81735f, -1.07735 };

	// Player Setting
	// Props Setting
	m_PlayerID = 0;
	m_MapName = MAP_STR_GAME_MAP;
	AppContext->DisplayProps(m_MapName);
//	AppContext->DisplayProps(m_MapName, true, 0.5f);

	m_Users[m_PlayerID] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
	m_Users[m_PlayerID]->m_PlayerID = m_PlayerID;
	m_Users[m_PlayerID]->m_MapName = m_MapName;
	m_Users[1] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN);

	///---
	// Player type, id 등등 세팅
	m_Users[m_PlayerID]->SetCamera(TOY_GROUND::GetApp()->m_Camera, CameraType::eThird);
	m_Users[m_PlayerID]->SetController();
	AppContext->DisplayCharacter(m_MapName, m_Users[m_PlayerID], 1);
	AppContext->DisplayCharacter(m_MapName, m_Users[1], 2);

	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eGamePlay);

	return false;
}

void GameplayScene::Exit()
{
	m_Users.clear();
	cout << "===========================================" << endl << endl;
}

void GameplayScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	for (auto& p : m_Users)
	{
		if (!p.second) continue;

		p.second->Update(fDeltaTime);
	}

	/* SkyCube */
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap["gameplaySky"], AppContext->m_RItemsVec);

	/*Props*/
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	/*Characters*/
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_COWBOY], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Cowboy, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_COWBOY].get());
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_GUNMAN], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::GunMan, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_GUNMAN].get());
	
	/*Materials*/
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void GameplayScene::Render()
{
	// Main rendering pass
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());

	// Props
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	// Charater
	for (auto& p : m_Users)
	{
		if (!p.second) continue;
		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedPSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}

	/*SkyBox*/
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkyPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap["gameplaySky"], AppContext->m_RItemsVec);
}

void GameplayScene::ChangeFreeCamera()
{
	if (TOY_GROUND::GetApp()->m_Camera->GetCameraType() == CameraType::eFree) return;

	XMFLOAT3 look = m_Users[m_PlayerID]->GetLook();
	XMFLOAT3 up = m_Users[m_PlayerID]->GetUp();
	XMFLOAT3 right = m_Users[m_PlayerID]->GetRight();

	TOY_GROUND::GetApp()->m_Camera->SetCamera(look, up, right);
}