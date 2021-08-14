#include "pch.h"
#include "MatchingRoomScene.h"
#include "Service.h"
#include "ApplicationContext.h"
#include "CommandContext.h"
#include "TOY_GROUND.h"
#include "InputHandler.h"
#include "AssertsReference.h"
#include "UserInterface.h"

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
	//AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_PLAY_BUTTON, TEXTURE_INDEX_UI_MATCHINGROOM_PLAY_RELEASED, TEXTURE_INDEX_UI_MATCHINGROOM_PLAY_PRESSED);
	//AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_EXIT_BUTTON, TEXTURE_INDEX_UI_MATCHINGROOM_EXIT_RELEASED, TEXTURE_INDEX_UI_MATCHINGROOM_EXIT_PRESSED);
	//AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_PLAY_BUTTON, TEXTURE_INDEX_UI_MATCHINGROOM_PLAY_RELEASED, TEXTURE_INDEX_UI_MATCHINGROOM_PLAY_PRESSED);
	//AppContext->CreateUI2D(ui2dName, OBJECT_NAME_MATCHINGROOM_EXIT_BUTTON, TEXTURE_INDEX_UI_MATCHINGROOM_EXIT_RELEASED, TEXTURE_INDEX_UI_MATCHINGROOM_EXIT_PRESSED);
}

void MatchingRoomScene::OnResize()
{
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eMatchingRoom);
}

bool MatchingRoomScene::Enter()
{
	cout << "=============== MatchingRoom Scene ===============" << endl;

	AppContext->DisplayUI2D(OBJECT_NAME_MATCHINGROOM_BACKGROUND, OBJECT_NAME_MATCHINGROOM_BACKGROUND, XMFLOAT2(0.f, 0.f), XMFLOAT2(1280, 720), TextAlignType::Center);

	// Logo
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND, XMFLOAT2(-650.f, 400.f), XMFLOAT2(400.f, 150.f), TextAlignType::Center);

	// Button
	//AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_PLAY_BUTTON, XMFLOAT2(0.f, -380.f), XMFLOAT2(420.f, 120.f), TextAlignType::Center);
	//AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_EXIT_BUTTON, XMFLOAT2(700.f, -380.f), XMFLOAT2(200.f, 90.f), TextAlignType::Center);

	return false;
}

void MatchingRoomScene::Exit()
{
	AppContext->HiddenUI2D(OBJECT_NAME_MATCHINGROOM_BACKGROUND, OBJECT_NAME_MATCHINGROOM_BACKGROUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_LOGO_TOYGROUND);
	//AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_PLAY_BUTTON);
	//AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_MATCHINGROOM_EXIT_BUTTON);

	cout << "===========================================" << endl << endl;
}

void MatchingRoomScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_MATCHINGROOM_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_MATCHINGROOM_BACKGROUND], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	/*Materials*/
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void MatchingRoomScene::Render()
{
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_MATCHINGROOM_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
}

void MatchingRoomScene::RenderText()
{
}
