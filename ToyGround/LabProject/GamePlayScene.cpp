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

	// 보석 생성
	AppContext->CreateGem();
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
#ifdef DEBUG_CLIENT
	m_PlayerID = 0;
#elif DEBUG_SERVER
	m_PlayerID = Service::GetApp()->GetMyBattleID();
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
	m_Users[m_PlayerID]->m_PlayerID = 0;
	m_Users[m_PlayerID]->m_MapName = m_MapName;
	m_Users[1] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN); 
	m_Users[1]->m_PlayerID = 1;
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

	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eGamePlay);

	// 보석 초기화
	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		AppContext->HiddenGem(i);
	}

	return false;
}

void GameplayScene::Exit()
{
	m_Users.clear();
	m_player_in_room.clear();
	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		AppContext->HiddenGem(i, true);
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
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_COWBOY], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Cowboy, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_COWBOY].get());
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_GUNMAN], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::GunMan, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_GUNMAN].get());
	
	// Gem
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_GEM], AppContext->m_RItemsVec);

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
	
	
	// Charater
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedPSO.Get());
	for (auto& p : m_Users)
	{
		if (!p.second) continue;
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}
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
	
	// Attack_Box
	for (auto& p : m_Users)
	{
		if (!p.second) continue;
		if (p.second->m_IsAiming)
		{
			GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OBBoxPSO.Get());
			GraphicsContext::GetApp()->DrawBoundingBox(AppContext->m_RItemsMap[OBJECT_MESH_STR_ATTACK_BOX], AppContext->m_RItemsVec, false);
		}
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