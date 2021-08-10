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
#include "UserInterface.h"
#include "Map.h"

void GameplayScene::ProcessEvent(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	case EVENT_ROOM_START: {
		cout << "씬 변경lobby -> Game, 서버에서 게임 정보 받아옴" << endl;
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
		break;
	}
	case EVENT_GAME_START: {
		cout << "EVENT_GAME_START(cnt down start)" << endl;
		va_list ap;
		int arg_bt_id;
		XMFLOAT4 arg_sl;
		va_start(ap, argsCount);
		arg_bt_id = va_arg(ap, int);
		arg_sl = va_arg(ap, XMFLOAT4);
		va_end(ap);

		//m_users 생성
		if (arg_bt_id == 1) {
			m_Users[arg_bt_id] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
		}
		else {
			m_Users[arg_bt_id] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN);
		}
		m_Users[arg_bt_id]->m_MapName = m_MapName;
		m_Users[arg_bt_id]->m_PlayerID = arg_bt_id;
		//m_Users[arg_bt_id]->m_SpawnLoaction = arg_sl;

		break;
	}
	case EVENT_GAME_ROUND_START: {
		m_PauseScene = false;
		break;
	}
	case EVENT_GAME_GAMEOVER: {
		int arg_winner;
		va_list ap;
		va_start(ap, argsCount);
		arg_winner = va_arg(ap, int);
		va_end(ap);

		for (int i = 0; i < MAX_GEM_COUNT; ++i)
			AppContext->HiddenGem(i, false);
		AppContext->HiddenProps(m_MapName);

		for (auto& u : m_Users) {
			if (u.second->m_PlayerID != arg_winner) {
				AppContext->HiddenCharacter(u.second);
			}
		}
		//todo Scene Change /*& show result*/
		SceneManager::GetApp()->ChangeScene(SceneType::eLobby);
		cout << "WINNER: " << arg_winner << endl;
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
	case EVENT_GAME_ADD_COIN: {
		XMFLOAT3 arg_pos;
		int arg_coin_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_pos = va_arg(ap, XMFLOAT3);
		arg_coin_id = va_arg(ap, int);
		va_end(ap);
		
		AppContext->DisplayGem(arg_coin_id, arg_pos.x, arg_pos.y, arg_pos.z);

		cout << "new Coin: [" << arg_pos.x << ", " << arg_pos.y << ", " << arg_pos.z << "]\n";
		break;
	}
	case EVENT_GAME_UPDATE_COIN: {
		int arg_id;
		int arg_coin_cnt;
		int arg_delete_coin_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_coin_cnt = va_arg(ap, int);
		arg_delete_coin_id = va_arg(ap, int);
		va_end(ap);
		cout << "update: " << arg_delete_coin_id << endl;
		AppContext->HiddenGem(arg_delete_coin_id, false);
		break;
	}
	case EVENT_GAME_CALLBACK_ANIM: {
		int arg_id;
		int arg_anim_type;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_anim_type = va_arg(ap, int);
		va_end(ap);
		//cout << "change anim id: " << arg_id << "anim: " << arg_anim_type << endl;
		m_Users[arg_id]->SetAnimationKeyState(static_cast<AnimationController::PlayerState>(arg_anim_type));
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

		if (m_Users[arg_id]) {
			m_Users[arg_id]->SetPosition(arg_pos.x, arg_pos.y, arg_pos.z);
		}
		break;
	}
	case EVENT_GAME_CALLBACK_MOUSE: {
		int arg_id;
		XMFLOAT3 arg_look;

		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_look = va_arg(ap, XMFLOAT3);
		va_end(ap);
		if (m_Users[arg_id] != nullptr) {
			m_Users[arg_id]->SetMatrixByLook(arg_look.x, arg_look.y, arg_look.z);
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

	// SkyCube
	AppContext->CreateSkycube("gameplaySky", "gameplaySky", "SkyBox");

	// 맵의 오브젝트들 생성
	AppContext->CreateProps(MAP_STR_GAME_MAP);

	// UI 생성
	AppContext->CreateUI2D(OBJECT_TYPE_AIM, OBJECT_NAME_GAMEPLAY_AIM, TEXTURE_INDEX_UI_GAMEPLAY_AIM);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER1_SCORE, TEXTURE_INDEX_UI_GAMEPLAY_PLAYER1_SCORE);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER2_SCORE, TEXTURE_INDEX_UI_GAMEPLAY_PLAYER2_SCORE);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_TIMER, TEXTURE_INDEX_UI_GAMEPLAY_TIMER);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_HEALTH, TEXTURE_INDEX_UI_GAMEPLAY_HEALTH);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE, TEXTURE_INDEX_UI_GAMEPLAY_ATTACK_GAUGE);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE, TEXTURE_INDEX_UI_GAMEPLAY_SKILL_GAUGE);

	// 보석 생성
	AppContext->CreateGem();

	// 총알 생성
	AppContext->CreateBullet();
}

void GameplayScene::OnResize()
{
	GraphicsContext::GetApp()->OnResizeBlur();
}

bool GameplayScene::Enter()
{
	cout << "============= Gameplay Scene ==============" << endl;

	// 마우스 보이게 하고싶다면 주석처리
	InputHandler::g_CursorSwitch = false;

	// Create SceneBounds for Shadow
	m_SceneBounds.Center = XMFLOAT3(0.f, 0.f, 0.f);
	m_SceneBounds.Radius = 3000.f;

	/* Light Setting */
	TOY_GROUND::GetApp()->m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 0.643f, -0.663f, -0.383f };
	TOY_GROUND::GetApp()->m_pLights[LIGHT_NAME_DIRECTIONAL]->Strength = { 0.75f, 0.75f, 0.75f };
	// Player Setting
#ifdef DEBUG_CLIENT
	m_PlayerID = 0;
	m_PauseScene = false;
#elif DEBUG_SERVER
	m_PlayerID = Service::GetApp()->GetMyBattleID();
	m_PauseScene = true;
#endif
	// Props Setting
	m_MapName = MAP_STR_GAME_MAP;
	AppContext->DisplayProps(m_MapName);
//	AppContext->DisplayProps(m_MapName, true, 0.5f);

#ifdef DEBUG_SERVER
	int cnt = Service::GetApp()->GetBattleClientsCount();
	m_player_in_room.resize(cnt);
	Service::GetApp()->AddEvent(EVENT_GAME_START);
#endif

#ifdef DEBUG_CLIENT
	m_Users[m_PlayerID] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
	m_Users[m_PlayerID]->m_IsVisible = true;
	m_Users[m_PlayerID]->m_IsVisibleOnePassCheck = true;
	m_Users[m_PlayerID]->m_PlayerID = 0;
	m_Users[m_PlayerID]->m_MapName = m_MapName;
	m_Users[1] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN); 
	m_Users[1]->m_PlayerID = 1;
	m_Users[1]->m_IsVisible = true;
	m_Users[1]->m_IsVisibleOnePassCheck = true;
	m_Users[1]->m_MapName = m_MapName;
#endif
	///---
	// Player type, id 등등 세팅
	for (auto& u : m_Users) {
		if (u.second) {
			AppContext->DisplayCharacter(m_MapName, u.second);
		}
	}
	m_Users[m_PlayerID]->SetCamera(TOY_GROUND::GetApp()->m_Camera, CameraType::eThird);
	m_Users[m_PlayerID]->SetController();

	// UI 세팅
	AppContext->DisplayUI2D(OBJECT_TYPE_AIM, OBJECT_NAME_GAMEPLAY_AIM, XMFLOAT2(-60.f, -80.f), XMFLOAT2(39, 39), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER1_SCORE, XMFLOAT2(-730.f, 450.f), XMFLOAT2(227, 60), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER2_SCORE, XMFLOAT2(730.f, 450.f), XMFLOAT2(227, 60), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_TIMER, XMFLOAT2(0.f, 450.f), XMFLOAT2(143, 47), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_HEALTH, XMFLOAT2(-730.f, -360.f), XMFLOAT2(245, 45), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE, XMFLOAT2(-730.f, -430.f), XMFLOAT2(245, 40), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE, XMFLOAT2(-730.f, -495.f), XMFLOAT2(245, 35), TextAlignType::Center);

	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eGamePlay);

	// 보석 초기화
	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		AppContext->HiddenGem(i);
	}

	// 총알 초기화
	for (int i = 0; i < MAX_BULLET_COUNT; ++i)
	{
		AppContext->HiddenBullet(i, false, 1);
		AppContext->HiddenBullet(i, false, 2);
	}

	return false;
}

void GameplayScene::Exit()
{
	m_Users.clear();
	m_player_in_room.clear();

	AppContext->HiddenUI2D(OBJECT_TYPE_AIM, OBJECT_NAME_GAMEPLAY_AIM);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER1_SCORE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER2_SCORE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_TIMER);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_HEALTH);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE);

	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		AppContext->HiddenGem(i, true);
	}

	for (int i = 0; i < MAX_BULLET_COUNT; ++i)
	{
		AppContext->HiddenBullet(i, false, 1);
		AppContext->HiddenBullet(i, false, 2);
	}

	cout << "===========================================" << endl << endl;
}

void GameplayScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	for (auto& p : m_Users)
	{
		if (!p.second) continue;

		if(p.second->m_PlayerController)
			p.second->m_PlayerController->m_PausGame = m_PauseScene;
		p.second->Update(fDeltaTime);
	}

	// SkyCube
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap["gameplaySky"], AppContext->m_RItemsVec);

	// Props
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	// Characters
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_COWBOY], AppContext->m_RItemsVec, true);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Cowboy, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_COWBOY].get());
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_GUNMAN], AppContext->m_RItemsVec, true);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::GunMan, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_GUNMAN].get());

	// Gem
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_GEM], AppContext->m_RItemsVec);

	// Bullet
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_01], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_02], AppContext->m_RItemsVec);

	// UI
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_AIM], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_AIM], AppContext->m_RItemsVec);

	// Shadow
	GraphicsContext::GetApp()->UpdateShadowTransform(TOY_GROUND::GetApp()->m_pLights[LIGHT_NAME_DIRECTIONAL].get(), m_SceneBounds);
	GraphicsContext::GetApp()->UpdateShadowPassCB();

	// Materials
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

	// Gem
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_MESH_STR_GEM], AppContext->m_RItemsVec);

	// Bullet
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_01], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_02], AppContext->m_RItemsVec);
	
	// AABoundingBox
	if (TOY_GROUND::GetApp()->bShowBoundingBox)
	{
		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_AABBoxPSO.Get());
		for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
		{
			GraphicsContext::GetApp()->DrawBoundingBox(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
		}
		for (auto& p : m_Users)
		{
			GraphicsContext::GetApp()->DrawBoundingBox(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
		}
	}
	
	// Charater
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedPSO.Get());
	for (auto& p : m_Users)
	{
		if (!p.second) continue;
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}
	
	bool AimCheck = false;
	// Attack_Box
	for (auto& p : m_Users)
	{
		if (!p.second) continue;
		if (p.second->m_IsAiming)
		{
			AimCheck = true;
			GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OBBoxPSO.Get());
			GraphicsContext::GetApp()->DrawBoundingBox(AppContext->m_RItemsMap[OBJECT_MESH_STR_ATTACK_BOX], AppContext->m_RItemsVec, false);
		}
	}

	// SkyBox
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkyPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap["gameplaySky"], AppContext->m_RItemsVec);

	// UI
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	if(!AimCheck)
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_AIM], AppContext->m_RItemsVec);
}

void GameplayScene::ChangeFreeCamera()
{
	if (TOY_GROUND::GetApp()->m_Camera->GetCameraType() == CameraType::eFree) return;

	XMFLOAT3 look = m_Users[m_PlayerID]->GetLook();
	XMFLOAT3 up = m_Users[m_PlayerID]->GetUp();
	XMFLOAT3 right = m_Users[m_PlayerID]->GetRight();

	TOY_GROUND::GetApp()->m_Camera->SetCamera(look, up, right);
}

void GameplayScene::RenderText()
{
	// Timer
	UITextInfo UITimer = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_GAMEPLAY_TIMER);
	GraphicsContext::GetApp()->SetTextSize(UITimer.size.y / 13.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
	GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);
	wstring TestTimerString;
	TestTimerString = L"2:00";
	GraphicsContext::GetApp()->DrawD2DText(TestTimerString, UITimer.size.x / 2.36f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);

	// HP
	UITextInfo UIHealth = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_GAMEPLAY_HEALTH);
	GraphicsContext::GetApp()->SetTextSize(UIHealth.size.y / 8.f , DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
	GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);
	wstring TestHPString;
	TestHPString = L"4000";
	GraphicsContext::GetApp()->DrawD2DText(TestHPString, UIHealth.size.x / 27.f, UIHealth.size.y * 1.3f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
}

void GameplayScene::WriteShadow()
{
	//
	// Shadow map pass.
	//
	GraphicsContext::GetApp()->SetResourceShadowPassCB();
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_ShadowOpaquePSO.Get());

	// Shadow Props
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	// Gem
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_MESH_STR_GEM], AppContext->m_RItemsVec);

	// Bullet
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_01], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_02], AppContext->m_RItemsVec);

	// Shadow Characters
	for (auto& p : m_Users)
	{
		if (!p.second) continue;

		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedShadowOpaquePSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}

	GraphicsContext::GetApp()->ShadowTransitionResourceBarrier();
}