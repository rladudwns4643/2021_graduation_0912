#include "pch.h"
#include "GameplayScene.h"
#include "TOY_GROUND.h"
#include "Timer.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "ApplicationContext.h"
#include "InputHandler.h"

#include "GameObject.h"
#include "Character.h"

void GameplayScene::Initialize()
{
	// Scene-Controller
	m_SceneController = new GameplayController(this);

	/* ���� ������Ʈ�� ���� */
	AppContext->CreateProps(MAP_STR_TOWN);
	AppContext->CreateContourProps(MAP_STR_TOWN);

	/* Param */
	m_PenaltyDuration = 0;
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
	// Props Setting
	m_PlayerID = 0;
	AppContext->DisplayProps(m_MapName);

	m_Users[m_PlayerID] = AppContext->FindObject<Character>(CHARACTER_WIZARD, CHARACTER_WIZARD);
	m_Users[m_PlayerID]->m_IsVisible = true;
	m_Users[m_PlayerID]->m_IsVisibleOnePassCheck = true;

	m_Users[1] = AppContext->FindObject<Character>(CHARACTER_BAIRD, CHARACTER_BAIRD);
	m_Users[1]->m_IsVisible = true;
	m_Users[1]->m_IsVisibleOnePassCheck = true;
	m_Users[2] = AppContext->FindObject<Character>(CHARACTER_DRUID, CHARACTER_DRUID);
	m_Users[2]->m_IsVisible = true;
	m_Users[2]->m_IsVisibleOnePassCheck = true;
	m_Users[3] = AppContext->FindObject<Character>(CHARACTER_SORCERER, CHARACTER_SORCERER);
	m_Users[3]->m_IsVisible = true;
	m_Users[3]->m_IsVisibleOnePassCheck = true;


	///---
	// Player type, id ��� ����
	// m_Users[]�� SetMapName ȣ���Ͽ� ĳ���Ͱ� �����ִ� ���̸� ����
	m_Users[m_PlayerID]->SetCamera(TOY_GROUND::GetApp()->m_pCamera, CameraType::eThird);
	m_Users[m_PlayerID]->SetController();

	// ī�޶� ����
	TOY_GROUND::GetApp()->m_pCamera->CameraInitialize(SceneType::eGamePlay);

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

	/*Props*/
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	/*Characters*/
	// ���߿� ������ �޽��� ������Ʈ �ϴ°ŷ� �����ҿ���
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_WIZARD], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_DRUID], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_BAIRD], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_FEMALE_PEASANT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_MALE_PEASANT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_SORCERER], AppContext->m_RItemsVec);

	/*Materials*/
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void GameplayScene::Render()
{
	//
	// Main rendering pass.
	//
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());
	/*Props*/
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	/*Characters*/
	for (auto& p : m_Users)
	{
		if (!p.second) continue;
		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}
}

void GameplayScene::ChangeFreeCamera()
{
	if (TOY_GROUND::GetApp()->m_pCamera->GetCameraType() == CameraType::eFree) return;


	XMFLOAT3 look = m_Users[m_PlayerID]->GetLook();
	XMFLOAT3 up = m_Users[m_PlayerID]->GetUp();
	XMFLOAT3 right = m_Users[m_PlayerID]->GetRight();

	TOY_GROUND::GetApp()->m_pCamera->SetCamera(look, up, right);
}