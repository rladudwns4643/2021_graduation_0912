#include "pch.h"
#include "GameplayScene.h"
#include "TOY_GROUND.h"
#include "Timer.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "ApplicationContext.h"
#include "InputHandler.h"
#include "Service.h"
#include "MoveCommand.h"
#include "CommandCenter.h"
#include "EnemyCommandCenter.h"
#include "ParticleSystem.h"

#include "GameObject.h"
#include "Character.h"
#include "UserInterface.h"
#include "Map.h"

void GameplayScene::ProcessEvent(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	case EVENT_ROOM_START: {
		//cout << "씬 변경lobby -> Game, 서버에서 게임 정보 받아옴" << endl;
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
		break;
	}
	case EVENT_GAME_START: {
#ifdef LOG_ON
		cout << "EVENT_GAME_START(cnt down start)" << endl;
#endif LOG_ON
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
		if (Service::GetApp()->GetMyBattleID() == 1) {
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Users[1]);
			EnemyCommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Users[2]);
		}
		else {
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Users[2]);
			EnemyCommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Users[1]);
		}
		break;
	}
	case EVENT_GAME_ROUND_START: {
		m_isCountDownOn = false;
		m_PauseScene = false;
		break;
	}
	case EVENT_GAME_GAMEOVER: {
		int arg_winner;
		va_list ap;
		va_start(ap, argsCount);
		arg_winner = va_arg(ap, int);
		va_end(ap);

		for (int i = 0; i < MAX_GEM_COUNT; ++i) {
			AppContext->HiddenGem(i, false);
		}
		AppContext->HiddenProps(m_MapName);

		for (auto& u : m_Users) {
			if (u.second->m_PlayerID != arg_winner) {
				AppContext->HiddenCharacter(u.second);
			}
		}
		m_IsGameOver = true;
		m_Winner = arg_winner;
		break;
	}
	case EVENT_GAME_TIMER: {
		va_list ap;
		int t;
		va_start(ap, argsCount);
		t = va_arg(ap, int);
		va_end(ap);
		m_Timer = t;
		if (m_StartCount > 0) {
			m_StartCount--;
		}
		if (m_isCountDownOn)
		{
			m_countDown--;

			if (m_countDownID == 1)
			{
				if(m_CowBoyGemNum < 10) CountDownReset();
				else if (m_CowBoyGemNum <= m_GunManGemNum) CountDownReset();
			}
			else
			{
				if (m_GunManGemNum < 10) CountDownReset();
				else if (m_CowBoyGemNum >= m_GunManGemNum) CountDownReset();
			}
		}
		else if(m_CowBoyGemNum >= 10 && m_GunManGemNum >= 10)
		{
			if (m_CowBoyGemNum < m_GunManGemNum)
			{
				m_isCountDownOn = true;
				m_countDownID = 2;
			}
			else
			{
				m_isCountDownOn = true;
				m_countDownID = 1;
			}
		}

		break;
	}
	case EVENT_GAME_WIN_SATISFACTION: {
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);

		//todo: 승리조건 달성한 id -> arg_id
		if (!m_isCountDownOn)
		{
			m_isCountDownOn = true;
			m_countDownID = arg_id;
		}

		cout << "WIN_SATISFACTION: " << arg_id << endl;
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

//#ifdef LOG_ON
		cout << "new Coin: [" << arg_pos.x << ", " << arg_pos.y << ", " << arg_pos.z << "]\n";
//#endif LOG_ON
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
//#ifdef LOG_ON
		cout << "Get coin ID: " << arg_id << " ID COIN COUNT: " << arg_coin_cnt << " delete CoinID: " << arg_delete_coin_id << endl;
//#endif LOG_ON
		if (arg_id == 1) {
			m_CowBoyGemNum = arg_coin_cnt;
		}
		else {
			m_GunManGemNum = arg_coin_cnt;
		}
		AppContext->HiddenGem(arg_delete_coin_id, false);
		break;
	}
	case EVENT_GAME_CALLBACK_HIT: {
		int arg_id;

		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		//m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_id);
		break;
	}
	case EVENT_GAME_CALLBACK_BULLET: {
		int arg_shootter;
		short arg_bullet_type;
		short arg_bullet_idx;
		XMFLOAT3 arg_cam_look;
		XMFLOAT3 arg_bullet_pos;

		va_list ap;
		va_start(ap, argsCount);
		arg_shootter = va_arg(ap, int);
		arg_bullet_type = va_arg(ap, short);
		arg_bullet_idx = va_arg(ap, short);
		arg_cam_look = va_arg(ap, XMFLOAT3);
		arg_bullet_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		//cout << "arg_bullet_idx: " << arg_bullet_idx << " arg_shootter: " << arg_shootter << endl;
		if (arg_bullet_type == 1) {
			AppContext->DisplayBullet(arg_bullet_idx, arg_bullet_pos, arg_cam_look, arg_shootter, arg_bullet_type);
			AppContext->m_ActiveBulletCheck[arg_bullet_idx] = true;
			AppContext->m_ActiveBullet[AppContext->m_ActiveBulletCnt] = arg_bullet_idx;
			AppContext->m_ActiveBulletCnt++;
		}
		else {
			AppContext->DisplayBullet(arg_bullet_idx, arg_bullet_pos, arg_cam_look, arg_shootter, arg_bullet_type);
			AppContext->m_AtiveSkillBulletCheck[arg_bullet_idx] = true;
			AppContext->m_AtiveSkillBullet[AppContext->m_ActiveBulletCnt] = arg_bullet_idx;
			AppContext->m_AtiveSkillBulletCnt++;
		}
		break;
	}
	case EVENT_GAME_CALLBACK_DIE: {
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);

		m_Users[arg_id]->Death();
		if (arg_id == 1) {
			m_CowBoyGemNum = 0;
		}
		else {
			m_GunManGemNum = 0;
		}

		break;
	}
	case EVENT_GAME_RESPAWN: {
		for (auto& a : m_Users) {
			a.second->Respawn();
		}

		break;
	}
	case EVENT_GAME_CALLBACK_PUSH_ANIM: {
		int arg_id;
		int arg_anim_type;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_anim_type = va_arg(ap, int);
		va_end(ap);
		//cout << "EVENT_GAME_CALLBACK_PUSH_ANIM: " << arg_id << "type: " << arg_anim_type << endl;
		EnemyCommandCenter::GetApp()->PushCommand<MoveCommand>(arg_anim_type, m_Users[arg_id]);
		if (arg_anim_type == static_cast<int>(MoveState::Jump)) {
			EnemyCommandCenter::GetApp()->m_StartJumpAnim = true;
		}
		if (arg_anim_type == static_cast<int>(MoveState::Attack)) {
			EnemyCommandCenter::GetApp()->m_StartAttackAnim = true;
		}
		break;
	}
	case EVENT_GAME_CALLBACK_POP_ANIM: {
		int arg_id;
		int arg_anim_type;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_anim_type = va_arg(ap, int);
		va_end(ap);
		//cout << "EVENT_GAME_CALLBACK_POP_ANIM: " << arg_id << "type: " << arg_anim_type << endl;
		EnemyCommandCenter::GetApp()->PopCommand(arg_anim_type);
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
	AppContext->CreateUI2D(OBJECT_TYPE_COUNTDOWN, OBJECT_NAME_GAMEPLAY_TIMER, TEXTURE_INDEX_UI_GAMEPLAY_TIMER);
	AppContext->CreateUI2D(OBJECT_TYPE_STATE_BACK, OBJECT_NAME_GAMEPLAY_STATE_BACK, TEXTURE_INDEX_UI_GAMEPLAY_STATE_BACK);
	AppContext->CreateUI2D(OBJECT_TYPE_ENEMY_STATE_BACK, OBJECT_NAME_GAMEPLAY_ENEMY_STATE_BACK, TEXTURE_INDEX_UI_GAMEPLAY_ENEMY_STATE_BACK);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_HEALTH, TEXTURE_INDEX_UI_GAMEPLAY_HEALTH);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE, TEXTURE_INDEX_UI_GAMEPLAY_ATTACK_GAUGE);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE, TEXTURE_INDEX_UI_GAMEPLAY_SKILL_GAUGE);
	AppContext->CreateUI2D(OBJECT_TYPE_STATE_FRONT, OBJECT_NAME_GAMEPLAY_STATE_FRONT, TEXTURE_INDEX_UI_GAMEPLAY_STATE_FRONT);
	AppContext->CreateUI2D(OBJECT_TYPE_ENEMY_STATE_FRONT, OBJECT_NAME_GAMEPLAY_ENEMY_STATE_FRONT, TEXTURE_INDEX_UI_GAMEPLAY_ENEMY_STATE_FRONT);
	AppContext->CreateUI2D(OBJECT_TYPE_WINNERBOARD, OBJECT_NAME_GAMEPLAY_WINNERBOARD, TEXTURE_INDEX_UI_GAMEPLAY_WINNERBOARD);
	AppContext->CreateUI2D(OBJECT_TYPE_WINNERBOARD2, OBJECT_NAME_GAMEPLAY_WINNERBOARD2, TEXTURE_INDEX_UI_GAMEPLAY_WINNERBOARD2);
	AppContext->CreateUI2D(OBJECT_TYPE_ENEMY_HEALTH, OBJECT_NAME_GAMEPLAY_ENEMY_HEALTH, TEXTURE_INDEX_UI_GAMEPLAY_HEALTH);

	// 보석 생성
	AppContext->CreateGem();

	// 총알 생성
	AppContext->CreateBullet();
	AppContext->BulletReset();
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
	m_PlayerID = 1;
	m_PauseScene = false;
#elif DEBUG_SERVER
	m_PlayerID = Service::GetApp()->GetMyBattleID();
	m_PauseScene = true;
#endif
	// Props Setting
	m_MapName = MAP_STR_GAME_MAP;
	AppContext->DisplayProps(m_MapName);

#ifdef DEBUG_SERVER
	int cnt = Service::GetApp()->GetBattleClientsCount();
	m_player_in_room.resize(cnt);
	Service::GetApp()->AddEvent(EVENT_GAME_START);
#endif
	m_IsGameOver = false;

#ifdef DEBUG_CLIENT
	m_Users[m_PlayerID] = AppContext->FindObject<Character>(CHARACTER_COWBOY, CHARACTER_COWBOY);
	m_Users[m_PlayerID]->m_IsVisible = true;
	m_Users[m_PlayerID]->m_IsVisibleOnePassCheck = true;
	m_Users[m_PlayerID]->m_PlayerID = 1;
	m_Users[m_PlayerID]->m_MapName = m_MapName;
	m_Users[2] = AppContext->FindObject<Character>(CHARACTER_GUNMAN, CHARACTER_GUNMAN); 
	m_Users[2]->m_PlayerID = 2;
	m_Users[2]->m_IsVisible = true;
	m_Users[2]->m_IsVisibleOnePassCheck = true;
	m_Users[2]->m_MapName = m_MapName;

	CommandCenter::GetApp()->ResetCommand();
	EnemyCommandCenter::GetApp()->ResetCommand();
	CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Users[1]);
	EnemyCommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Users[2]);
#endif:

	m_Users[m_PlayerID]->SetCamera(TOY_GROUND::GetApp()->m_Camera, CameraType::eThird);
	m_Users[m_PlayerID]->SetController();
	///---
	// Player type, id 등등 세팅
	for (auto& u : m_Users) {
		if (u.second) {
			AppContext->DisplayCharacter(m_MapName, u.second);
		}
	}

	// UI 세팅
	AppContext->DisplayUI2D(OBJECT_TYPE_AIM, OBJECT_NAME_GAMEPLAY_AIM, XMFLOAT2(-60.f, -80.f), XMFLOAT2(39, 39), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER1_SCORE, XMFLOAT2(-730.f, 450.f), XMFLOAT2(227, 60), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER2_SCORE, XMFLOAT2(730.f, 450.f), XMFLOAT2(227, 60), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_COUNTDOWN, OBJECT_NAME_GAMEPLAY_TIMER, XMFLOAT2(0.f, 450.f), XMFLOAT2(143, 47), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_HEALTH, XMFLOAT2(-730.f, -360.f), XMFLOAT2(245, 45), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE, XMFLOAT2(-730.f, -430.f), XMFLOAT2(245, 40), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE, XMFLOAT2(-730.f, -495.f), XMFLOAT2(0, 35), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_STATE_FRONT, OBJECT_NAME_GAMEPLAY_STATE_FRONT, XMFLOAT2(-730.f, -423.f), XMFLOAT2(255, 140), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_STATE_BACK, OBJECT_NAME_GAMEPLAY_STATE_BACK, XMFLOAT2(-730.f, -423.f), XMFLOAT2(255, 140), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_ENEMY_STATE_FRONT, OBJECT_NAME_GAMEPLAY_ENEMY_STATE_FRONT, XMFLOAT2(730.f, -423.f), XMFLOAT2(255, 140), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_ENEMY_STATE_BACK, OBJECT_NAME_GAMEPLAY_ENEMY_STATE_BACK, XMFLOAT2(730.f, -423.f), XMFLOAT2(255, 140), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_WINNERBOARD, OBJECT_NAME_GAMEPLAY_WINNERBOARD, XMFLOAT2(0, 0), XMFLOAT2(500.f, 150.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_WINNERBOARD2, OBJECT_NAME_GAMEPLAY_WINNERBOARD2, XMFLOAT2(0, 0), XMFLOAT2(500.f, 150.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_ENEMY_HEALTH, OBJECT_NAME_GAMEPLAY_ENEMY_HEALTH, XMFLOAT2(730.f, -360.f), XMFLOAT2(245, 45), TextAlignType::Center, -1, true);


	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eGamePlay);
	TOY_GROUND::GetApp()->m_Camera->Update();

	// 보석 초기화
	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		AppContext->HiddenGem(i);
	}

	// 총알 초기화
	for (int i = 0; i < MAX_BULLET_COUNT; ++i)
	{
		AppContext->HiddenBullet(i, 1);
		AppContext->HiddenBullet(i, 2);
	}
	AppContext->BulletReset();
	
	m_IsGameOver = false;
	m_CowBoyGemNum = 0;
	m_GunManGemNum = 0;
	m_StartCount = 4;
	CountDownReset();

	return false;
}

void GameplayScene::Exit()
{
	if(m_Users[1]->m_isLive == false)
		m_Users[1]->Respawn();
	if (m_Users[2]->m_isLive == false)
		m_Users[2]->Respawn();

	m_Users.clear();
	m_player_in_room.clear();

	AppContext->HiddenUI2D(OBJECT_TYPE_AIM, OBJECT_NAME_GAMEPLAY_AIM);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER1_SCORE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_PLAYER2_SCORE);
	AppContext->HiddenUI2D(OBJECT_TYPE_COUNTDOWN, OBJECT_NAME_GAMEPLAY_TIMER);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_HEALTH);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE);
	AppContext->HiddenUI2D(OBJECT_TYPE_STATE_FRONT, OBJECT_NAME_GAMEPLAY_STATE_FRONT);
	AppContext->HiddenUI2D(OBJECT_TYPE_STATE_BACK, OBJECT_NAME_GAMEPLAY_STATE_BACK);
	AppContext->HiddenUI2D(OBJECT_TYPE_ENEMY_STATE_FRONT, OBJECT_NAME_GAMEPLAY_ENEMY_STATE_FRONT);
	AppContext->HiddenUI2D(OBJECT_TYPE_ENEMY_STATE_BACK, OBJECT_NAME_GAMEPLAY_ENEMY_STATE_BACK);
	AppContext->HiddenUI2D(OBJECT_TYPE_WINNERBOARD, OBJECT_NAME_GAMEPLAY_WINNERBOARD);
	AppContext->HiddenUI2D(OBJECT_TYPE_WINNERBOARD2, OBJECT_NAME_GAMEPLAY_WINNERBOARD2);
	AppContext->HiddenUI2D(OBJECT_TYPE_ENEMY_HEALTH, OBJECT_NAME_GAMEPLAY_ENEMY_HEALTH);

	// 파티클
	AppContext->HiddenParticle(PARTICLE_NAME_SKILL_ON_CHARACTER, CHARACTER_COWBOY);
	AppContext->HiddenParticle(PARTICLE_NAME_SKILL_ON_CHARACTER, CHARACTER_GUNMAN);
	AppContext->HiddenParticle(PARTICLE_NAME_DAMAGE_CHARACTER, CHARACTER_COWBOY);
	AppContext->HiddenParticle(PARTICLE_NAME_DAMAGE_CHARACTER, CHARACTER_GUNMAN);
	AppContext->HiddenParticle(PARTICLE_NAME_SMOKE, PARTICLE_NAME_SMOKE);

	for (int i = 0; i < MAX_GEM_COUNT; ++i)
	{
		AppContext->HiddenGem(i, true);
	}

	for (int i = 0; i < MAX_BULLET_COUNT; ++i)
	{
		AppContext->HiddenBullet(i, 1);
		AppContext->HiddenBullet(i, 2);
	}
	AppContext->BulletReset();

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
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Cowboy, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_COWBOY].get(), 1);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_COWBOY], AppContext->m_RItemsVec, true);
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::GunMan, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_GUNMAN].get(), 2);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_GUNMAN], AppContext->m_RItemsVec, true);

	// Gem
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_GEM], AppContext->m_RItemsVec);

	// Bullet
	AppContext->UpdateBullet();
	GraphicsContext::GetApp()->UpdateBulletInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_01], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateBulletInstanceData(AppContext->m_RItemsMap[OBJECT_MESH_STR_BULLET_02], AppContext->m_RItemsVec);

	// UI
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_AIM], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_AIM], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_STATE_FRONT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_STATE_FRONT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_STATE_BACK], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_STATE_BACK], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_WINNERBOARD], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_WINNERBOARD], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_WINNERBOARD2], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_WINNERBOARD2], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_COUNTDOWN], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_COUNTDOWN], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_HEALTH], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_HEALTH], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_FRONT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_FRONT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_BACK], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_BACK], AppContext->m_RItemsVec);
	

	// Particle
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_SKILL_ON_CHARACTER], AppContext->m_RItemsVec, false, true);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_SMOKE], AppContext->m_RItemsVec, false, true);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_DAMAGE_CHARACTER], AppContext->m_RItemsVec, false, true);

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

	// Particle
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_ParticlePSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_SMOKE], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_SKILL_ON_CHARACTER], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_DAMAGE_CHARACTER], AppContext->m_RItemsVec);

	// UI
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_STATE_BACK], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	if (!AimCheck && m_IsGameOver == false) {
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_AIM], AppContext->m_RItemsVec);
	}
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_STATE_FRONT], AppContext->m_RItemsVec);
	if (m_isCountDownOn == true)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_COUNTDOWN], AppContext->m_RItemsVec);
	}
	if (m_IsGameOver == true) 
	{
		if(m_Winner == 1)
			GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_WINNERBOARD], AppContext->m_RItemsVec);
		else
			GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_WINNERBOARD2], AppContext->m_RItemsVec);
	}

	// Enemy State
#ifdef DEBUG_CLIENT
	if (m_Users[2]->m_hp < MAX_HP && m_Users[2]->m_isLive)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_BACK], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_HEALTH], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_FRONT], AppContext->m_RItemsVec);
	}
#elif DEBUG_SERVER
	if (m_Users[NetCore::GetApp()->GetBattleID() % 2 + 1]->m_hp < MAX_HP && m_Users[NetCore::GetApp()->GetBattleID() % 2 + 1]->m_isLive)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_BACK], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_HEALTH], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ENEMY_STATE_FRONT], AppContext->m_RItemsVec);
	}
#endif
}

void GameplayScene::RenderText()
{
	// Timer
	if (m_isCountDownOn == true)
	{
		UITextInfo UITimer = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_COUNTDOWN], AppContext->m_RItemsVec, OBJECT_NAME_GAMEPLAY_TIMER);
		GraphicsContext::GetApp()->SetTextSize(UITimer.size.y / 13.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
		GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);
		auto t = to_wstring(m_countDown);
		if (t.size() > 1)
			GraphicsContext::GetApp()->DrawD2DText(t, UITimer.size.x / 2.29f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		else
			GraphicsContext::GetApp()->DrawD2DText(t, UITimer.size.x / 2.27f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	}

	// Score
	UITextInfo UITimer = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_COUNTDOWN], AppContext->m_RItemsVec, OBJECT_NAME_GAMEPLAY_TIMER);
	GraphicsContext::GetApp()->SetTextSize(UITimer.size.y / 13.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
	GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);
	auto cs = to_wstring(m_CowBoyGemNum);
	auto gs = to_wstring(m_GunManGemNum);
	if(cs.size() > 1)
		GraphicsContext::GetApp()->DrawD2DText(cs, UITimer.size.x * 0.033f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	else
		GraphicsContext::GetApp()->DrawD2DText(cs, UITimer.size.x * 0.038f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	if(gs.size() > 1)
		GraphicsContext::GetApp()->DrawD2DText(gs, UITimer.size.x * 0.837f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	else
		GraphicsContext::GetApp()->DrawD2DText(gs, UITimer.size.x * 0.842f, UITimer.pos.y / 5.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);

	UITextInfo UIHealth = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_GAMEPLAY_HEALTH);
	// StartCount
#ifdef DEBUG_SERVER
	if (m_StartCount > 0)
	{
		auto sc = to_wstring(m_StartCount);
		GraphicsContext::GetApp()->SetTextSize(UIHealth.size.y / 3.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
		GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);
		GraphicsContext::GetApp()->DrawD2DText(sc, Core::g_DisplayWidth / 2 - 20, Core::g_DisplayHeight / 2, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	}
#endif

	// HP
	GraphicsContext::GetApp()->SetTextSize(UIHealth.size.y / 9.f , DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
	GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);

#ifdef DEBUG_CLIENT
	auto hp = to_wstring(m_Users[1]->m_hp);
	auto ehp = to_wstring(m_Users[2]->m_hp);
#elif DEBUG_SERVER
	auto hp = to_wstring(m_Users[NetCore::GetApp()->GetBattleID()]->m_hp);
	auto ehp = to_wstring(m_Users[NetCore::GetApp()->GetBattleID() % 2 + 1]->m_hp);
#endif
	float hpPosX = UIHealth.size.y * 0.21f;
	for(int i = 0; i < 4 - hp.size(); ++i)
		hpPosX += 15;

	float ehpPosX = UIHealth.size.y * 2.38f;
	for (int i = 0; i < 4 - ehp.size(); ++i)
		ehpPosX += 15;
	GraphicsContext::GetApp()->DrawD2DText(hp, hpPosX, UIHealth.size.y * 1.33f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
#ifdef DEBUG_CLIENT
	if (m_Users[2]->m_hp < MAX_HP && m_Users[2]->m_isLive)
	{
		GraphicsContext::GetApp()->DrawD2DText(ehp, ehpPosX, UIHealth.size.y * 1.33f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		wstring en;
		string pn = m_Users[2]->GetMeshName();
		pn += " HP";
		en.assign(pn.begin(), pn.end());
		GraphicsContext::GetApp()->SetTextSize(UIHealth.size.y / 12.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
		GraphicsContext::GetApp()->DrawD2DText(en, ehpPosX * 0.95f, UIHealth.size.y * 1.43f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	}
#elif DEBUG_SERVER
	if (m_Users[NetCore::GetApp()->GetBattleID() % 2 + 1]->m_hp < MAX_HP && &&m_Users[NetCore::GetApp()->GetBattleID() % 2 + 1]->m_isLive)
	{
		GraphicsContext::GetApp()->DrawD2DText(ehp, ehpPosX, UIHealth.size.y * 1.33f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		wstring en;
		string pn = m_Users[NetCore::GetApp()->GetBattleID() % 2 + 1]->GetMeshName();
		pn += " HP";
		en.assign(pn.begin(), pn.end());
		GraphicsContext::GetApp()->SetTextSize(UIHealth.size.y / 12.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
		GraphicsContext::GetApp()->DrawD2DText(en, ehpPosX * 0.95f, UIHealth.size.y * 1.43f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	}
#endif
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
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec, true);
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