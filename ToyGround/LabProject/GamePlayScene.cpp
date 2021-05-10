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
	case EVENT_ROOM_START: {
		cout << "�� ����lobby -> Game, �������� ���� ���� �޾ƿ�" << endl;
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
		break;
	}
	case EVENT_GAME_START: {
		cout << "���� ����" << endl;
		va_list ap;
		int arg_bt_id;
		XMFLOAT4 arg_sl;
		va_start(ap, argsCount);
		arg_bt_id = va_arg(ap, int);
		arg_sl = va_arg(ap, XMFLOAT4);
		va_end(ap);

		//m_users ����
		if (arg_bt_id == 1) {
			m_Users[arg_bt_id] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
		}
		else {
			m_Users[arg_bt_id] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN);
		}
		m_Users[arg_bt_id]->m_PlayerID = arg_bt_id;
		m_Users[arg_bt_id]->m_SpawnLoaction = arg_sl;

		break;
	}
	case EVENT_GAME_TIMER: {
		va_list ap;
		int t;
		va_start(ap, argsCount);
		t = va_arg(ap, int);
		va_end(ap);
		cout << "left Time: " << t << endl;
		break;
	}
	case EVENT_GAME_CALLBACK_MOVE: {
		int arg_id;
		XMFLOAT3 arg_pos;

		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		if (m_Users[arg_id] != nullptr) {
			m_Users[arg_id]->SetPosition(arg_pos.x, arg_pos.y, arg_pos.z);
		}
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

	/* ���� ������Ʈ�� ���� */
	AppContext->CreateProps(MAP_STR_GAME_MAP);
}

void GameplayScene::OnResize()
{
}

bool GameplayScene::Enter()
{
	cout << "============= Gameplay Scene ==============" << endl;

	// �����÷��� ������ ���콺 ���̰� �ϰ�ʹٸ� �ּ�ó��
	InputHandler::g_CursorSwitch = false;

	/* Create SceneBounds for Shadow */
	m_SceneBounds.Center = XMFLOAT3(2500.f, 0.0f, 2500.f);
	m_SceneBounds.Radius = 2700.f;

	/* Light Setting */
	TOY_GROUND::GetApp()->m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 0.57735f, -0.81735f, -1.07735 };

	// Player Setting
	m_PlayerID = Service::GetApp()->GetMyBattleID();
	// Props Setting
	m_MapName = MAP_STR_GAME_MAP;
	AppContext->DisplayProps(m_MapName);
//	AppContext->DisplayProps(m_MapName, true, 0.5f);

	Service::GetApp()->Notify(EVENT_GAME_START);

	//m_Users[m_PlayerID] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
	//m_Users[m_PlayerID]->m_PlayerID = m_PlayerID;
	//m_Users[m_PlayerID]->m_MapName = m_MapName;
	//m_Users[1] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN);

	///---
	// Player type, id ��� ����
	for (auto& u : m_Users) {
		if (u.second) {
			AppContext->DisplayCharacter(m_MapName, u.second, u.second->m_SpawnLoaction);
		}
	}
	m_Users[m_PlayerID]->SetCamera(TOY_GROUND::GetApp()->m_Camera, CameraType::eThird);
	m_Users[m_PlayerID]->SetController();

	// ī�޶� ����
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