#include "pch.h"
#include "MatchingRoomScene.h"
#include "Service.h"
#include "ApplicationContext.h"
#include "CommandContext.h"
#include "TOY_GROUND.h"
#include "InputHandler.h"
#include "AssertsReference.h"
#include "UserInterface.h"

#include "Character.h"
#include "GameObject.h"

#define WIDTH_NORMALIZE_UI(x) (x + (Core::g_DisplayWidth / 2.f))
#define HEIGHT_NORMALIZE_UI(y) (-y + (Core::g_DisplayHeight / 2.f))

void MatchingRoomScene::Initialize()
{
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eMatchingRoom);

	m_SceneController = new MatchingRoomController(this);
	m_SceneController->SetMapName("");

	std::string ui2dName = OBJECT_TYPE_UI2D + m_SceneName;
	AppContext->CreateUI2D(OBJECT_NAME_MATCHINGROOM_BACKGROUND, OBJECT_NAME_MATCHINGROOM_BACKGROUND, TEXTURE_INDEX_UI_MATCHINGROOM_BACKGROUND);

	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND, TEXTURE_INDEX_UI_MATCHINGROOM_LOGO_TOYGROUND);

	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_R, TEXTURE_INDEX_UI_MATCHINGROOM_READY1_RELEASED, TEXTURE_INDEX_UI_MATCHINGROOM_READY1_RELEASED);
	AppContext->CreateUI2D(OBJECT_TYPE_READY1_PRESSED, OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_P, TEXTURE_INDEX_UI_MATCHINGROOM_READY1_PRESSED, TEXTURE_INDEX_UI_MATCHINGROOM_READY1_PRESSED);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_R, TEXTURE_INDEX_UI_MATCHINGROOM_READY2_RELEASED, TEXTURE_INDEX_UI_MATCHINGROOM_READY2_RELEASED);
	AppContext->CreateUI2D(OBJECT_TYPE_READY2_PRESSED, OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_P, TEXTURE_INDEX_UI_MATCHINGROOM_READY2_PRESSED, TEXTURE_INDEX_UI_MATCHINGROOM_READY2_PRESSED);
}

void MatchingRoomScene::OnResize()
{
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eMatchingRoom);
}

bool MatchingRoomScene::Enter()
{
	cout << "=============== MatchingRoom Scene ===============" << endl;

	InputHandler::g_CursorSwitch = true;

	m_Toys[0] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
	m_Toys[0]->m_IsVisible = true;
	m_Toys[0]->m_IsVisibleOnePassCheck = true;
	m_Toys[0]->InitializeTransform();
	m_Toys[0]->SetAnimationKeyState(AnimationController::PlayerState::STATE_T_POSE);
	m_Toys[0]->SetAnimationPlayerState(AnimationController::PlayerState::STATE_T_POSE);
	m_Toys[0]->SetMatrixByLook(0, 0, 1);
	m_Toys[0]->SetPosition(230.f, -100.f, 100.f);
	m_Toys[0]->m_PlayerID = 100;

	m_Toys[1] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN);
	m_Toys[1]->m_IsVisible = true;
	m_Toys[1]->m_IsVisibleOnePassCheck = true;
	m_Toys[1]->InitializeTransform();
	m_Toys[1]->SetAnimationKeyState(AnimationController::PlayerState::STATE_T_POSE);
	m_Toys[1]->SetAnimationPlayerState(AnimationController::PlayerState::STATE_T_POSE);
	m_Toys[1]->SetMatrixByLook(0, 0, 1);
	m_Toys[1]->SetPosition(-230.f, -100.f, 100.f);
	m_Toys[1]->m_PlayerID = 100;

	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eMatchingRoom);

	m_ScaleConvert = XMFLOAT2(1.f, 1.f);

	AppContext->DisplayUI2D(OBJECT_NAME_MATCHINGROOM_BACKGROUND, OBJECT_NAME_MATCHINGROOM_BACKGROUND, XMFLOAT2(0.f, 0.f), XMFLOAT2(1280, 720), TextAlignType::Center);

	// Logo
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND, XMFLOAT2(0.f, 400.f), XMFLOAT2(414.f, 162.f), TextAlignType::Center, -1, true);

	// Button
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_R, XMFLOAT2(-525.f, -430.f), XMFLOAT2(300.f, 90.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_R, XMFLOAT2(525.f, -430.f), XMFLOAT2(300.f, 90.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_READY1_PRESSED, OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_P, XMFLOAT2(-525.f, -430.f), XMFLOAT2(300.f, 90.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_READY2_PRESSED, OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_P, XMFLOAT2(525.f, -430.f), XMFLOAT2(300.f, 90.f), TextAlignType::Center);

	return false;
}

void MatchingRoomScene::Exit()
{
	m_Toys.clear();

	AppContext->HiddenUI2D(OBJECT_NAME_MATCHINGROOM_BACKGROUND, OBJECT_NAME_MATCHINGROOM_BACKGROUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_R);
	AppContext->HiddenUI2D(OBJECT_TYPE_READY1_PRESSED, OBJECT_NAME_MATCHINGROOM_READY1_BUTTON_P);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_R);
	AppContext->HiddenUI2D(OBJECT_TYPE_READY2_PRESSED, OBJECT_NAME_MATCHINGROOM_READY2_BUTTON_P);

	cout << "===========================================" << endl << endl;
}

void MatchingRoomScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	TOY_GROUND::GetApp()->m_Camera->SetTarget(XMFLOAT3(0, 0, -1));

	// Character
	for (auto& p : m_Toys)
	{
		if (!p.second) continue;

		p.second->Update(fDeltaTime);
	}
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Cowboy, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_COWBOY].get(), 1);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_COWBOY], AppContext->m_RItemsVec, true);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::GunMan, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_GUNMAN].get(), 2);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_GUNMAN], AppContext->m_RItemsVec, true);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_MATCHINGROOM_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_MATCHINGROOM_BACKGROUND], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	if (m_isReady1)
	{
		GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_READY1_PRESSED], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_READY1_PRESSED], AppContext->m_RItemsVec);
	}
	if (m_isReady2)
	{
		GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_READY2_PRESSED], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_READY2_PRESSED], AppContext->m_RItemsVec);
	}
	// Materials
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void MatchingRoomScene::Render()
{
	// Character
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedPSO.Get());
	for (auto& p : m_Toys)
	{
		if (!p.second) continue;
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}

	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_MATCHINGROOM_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	if (m_isReady1)
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_READY1_PRESSED], AppContext->m_RItemsVec);
	if (m_isReady2)
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_READY2_PRESSED], AppContext->m_RItemsVec);
}

void MatchingRoomScene::RenderText()
{
	UITextInfo LogoUI = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND);
	//cout << "x: " << LogoUI.size.x << ", ";
	//cout << "y: " << LogoUI.size.y << endl;
	m_ScaleConvert.x = LogoUI.size.x / 4140;
	m_ScaleConvert.y = LogoUI.size.y / 1620;
}
