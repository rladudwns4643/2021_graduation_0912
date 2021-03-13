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

void GameplayScene::ProcessEvent(int sEvent, int argsCount, ...)
{
	switch (sEvent)
	{
	case  EVENT_GAMEPLAY_START:
	{
		int argBattleID;
		int argPlayerType; /*����: ���� / �л�*/
		std::string argPlayerMesh;
		int argSpawnLocation;

		va_list ap;
		va_start(ap, argsCount);
		argBattleID = va_arg(ap, int);
		argPlayerType = va_arg(ap, int);
		argPlayerMesh = va_arg(ap, std::string);
		argSpawnLocation = va_arg(ap, int);
		va_end(ap);

		if (argPlayerType == ROLE_MASTER) {
			m_PlayerIDs[0] = argBattleID;
			m_Users[argBattleID] = AppContext->FindObject<Character>(CHARACTER_WIZARD, CHARACTER_WIZARD);
		}
		else {
			m_PlayerIDs[m_IndexSaveForPlayerID++] = argBattleID;
			m_PlayerNames.emplace_back(argPlayerMesh);
			m_Users[argBattleID] = AppContext->FindObject<Character>(argPlayerMesh, argPlayerMesh);
		}

		if (m_Users[argBattleID]) {
			m_Users[argBattleID]->m_BattleServerID = argBattleID;
			m_Users[argBattleID]->m_PlayerRole = argPlayerType;
			m_Users[argBattleID]->m_SpawnLoaction = argSpawnLocation;
			m_Users[argBattleID]->SetMapName(m_MapName);
		}
		else
		{
			cout << "m_Users�� �������� �ʽ��ϴ�." << endl;
			cout << "[" << argBattleID << "] - PlayerType: " << argPlayerType << " / PlayerMesh: " << argPlayerMesh << endl;
		}

		break;
	}
	case EVENT_GAMEPLAY_ROUND_START:
	{
		CommandCenter::GetApp()->m_AttackCoolTimeSwitch = true;
		break;
	}
	case EVENT_GAMEPLAY_CALLBACK_MOVE:
	{
		int argID;
		XMFLOAT3 argPos;

		va_list ap;
		va_start(ap, argsCount);
		argID = va_arg(ap, int);
		argPos = va_arg(ap, DirectX::XMFLOAT3);
		va_end(ap);

		if (m_Users[argID])
			m_Users[argID]->SetPosition(argPos.x, argPos.y, argPos.z);
		break;
	}
	case EVENT_GAMEPLAY_CALLBACK_MOUSE:
	{
		int argID;
		XMFLOAT3 argLook;

		va_list ap;
		va_start(ap, argsCount);
		argID = va_arg(ap, int);
		argLook = va_arg(ap, XMFLOAT3);
		va_end(ap);

		if (m_Users[argID])
			m_Users[argID]->SetMatrixByLook(argLook.x, argLook.y, argLook.z);

		break;
	}
	case EVENT_GAMEPLAY_OBJECT_POS:
	{
		std::string argMeshName;
		std::string argInstID;
		XMFLOAT3 argObjPos;

		va_list ap;
		va_start(ap, argsCount);
		argMeshName = va_arg(ap, std::string);
		argInstID = va_arg(ap, std::string);
		argObjPos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		string type = AppContext->FindClientInstanceID(m_MapName, argMeshName);
		if (type != "") {
			GameObject* p = AppContext->FindObject<GameObject>(type, argInstID);
			if (p) p->SetPosition(argObjPos);
			else cout << "error none id" << endl;
		}
		else
			cout << "None Key" << endl;

		break;
	}
	case EVENT_GAMEPLAY_TRAMNSFORM_RECV:
	{
		int argClientID;
		std::string serverMeshID;

		va_list ap;
		va_start(ap, argsCount);
		argClientID = va_arg(ap, int);
		serverMeshID = va_arg(ap, std::string);
		va_end(ap);

		if (serverMeshID == std::to_string(OBJECT_TYPE_CHARACTER))
		{
			if (!m_Users[argClientID]) return;

			bool isReleaseTransform = m_Users[argClientID]->ReleaseTransform();

			// ���������Ҹ�
			if (isReleaseTransform)
			{
				SoundManager::GetApp()->PlaySoundOnce(L"EFF_Player_Transform.wav", SoundManager::CHANNEL_ID::PLAYER_TRANSFORM, 0.6f);
				// �������� ��ƼŬ
				m_Users[argClientID]->TransformParticle();
			}
		}
		else
		{
			Map* tMap = AppContext->m_Maps[m_MapName];
			string type = tMap->mapInfoDic[serverMeshID];
			bool isTransform = m_Users[argClientID]->Transform(type, type, tMap->propTexture);

			if (isTransform && (type != m_Users[argClientID]->m_TransformMeshName))
			{
				// ���� �Ҹ�
				SoundManager::GetApp()->PlaySoundOnce(L"EFF_Player_Transform.wav", SoundManager::CHANNEL_ID::PLAYER_TRANSFORM, 0.6f);
				// ���� ��ƼŬ
				m_Users[argClientID]->TransformParticle();
			}

			m_Users[argClientID]->m_TransformMeshName = type;
		}

		break;
	}
	case EVENT_GAMEPLAY_PUT_THUNDERBOLT:
	{
		int boltID;
		XMFLOAT3 putPos;

		va_list ap;
		va_start(ap, argsCount);
		boltID = va_arg(ap, int);
		putPos = va_arg(ap, XMFLOAT3);
		va_end(ap);

		// Play Attack Sound
		SoundManager::GetApp()->PlaySoundOnce(L"EFF_Player_Attack.wav", SoundManager::CHANNEL_ID::PLAYER_ATTACK, 0.8f);
		AppContext->DisplayThunderBolt(boltID, putPos.x, putPos.y, putPos.z);
		AppContext->DisplayParticle(PARTICLE_NAME_THUNDER, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + boltID), XMFLOAT3(putPos.x, putPos.y, putPos.z), true);

		break;
	}
	case EVENT_GAMEPLAY_REMOVE_THUNDERBOLT:
	{
		int boltID;

		va_list ap;
		va_start(ap, argsCount);
		boltID = va_arg(ap, int);
		va_end(ap);

		// �浹��ƼŬ
		AppContext->DisplayParticle(PARTICLE_NAME_LIGHT_EXPLOSION, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + boltID), XMFLOAT3(0, 0, 0), true, true);

		// boltID , removePos�� �̿��Ͽ� ������? ȿ���� �ְ� ȭ�鿡�� ����� �˴ϴ�.
		// ������ ȿ���� ���� -> ThunderBolt��� Ŭ������ �߰�����.
		AppContext->HiddenThunderBolt(boltID);
		AppContext->HiddenParticle(PARTICLE_NAME_THUNDER, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + boltID));

		// �´� ���� �߰�

		break;
	}
	case EVENT_GAMEPLAY_ANIMATE:
	{
		int id; // �ִϸ��̼� ������ ��ƲŬ���̾�Ʈ
		char anim_type; // ������ �ִϸ��̼� Ÿ��

		va_list ap;
		va_start(ap, argsCount);
		id = va_arg(ap, int);
		anim_type = va_arg(ap, char);
		va_end(ap);

		// 20.04.09 ������ - �ִϸ��̼� �����ϱ�. ( �������ݿ� ����� ANIMATION TYPES ���� )
		Character* user = m_Users[id];
		if (!user) break;

		int chrState = AppContext->FindAnimName(user->m_PlayerRole, anim_type);
		user->SetAnimationKeyState(static_cast<AnimationController::PlayerState>(chrState));
		break;
	}
	case EVENT_GAMEPLAY_HIT:
	{
		int id;
		double hp;

		va_list ap;
		va_start(ap, argsCount);
		id = va_arg(ap, int);
		hp = va_arg(ap, double);
		va_end(ap);

		hp *= 0.559;

		if (m_Users[id])
		{
			m_Users[id]->m_HP = 0.48f + (float)hp;
			if (m_Users[id]->m_HP > 1.f)
				m_Users[id]->m_HP = 1.f;
			cout << "HP - " << m_Users[id]->m_HP << endl;
		}

		// Sound
		SoundManager::GetApp()->PlaySoundOnce(L"EFF_Player_Beat.wav", SoundManager::CHANNEL_ID::PLAYER_BEAT, 0.6f);

		// Blur On
		if (id == m_PlayerID)
		{
			m_Users[m_PlayerID]->ControlBlurEffect(true);
		}

		break;
	}
	case EVENT_GAMEPLAY_PLAYER_DIE:
	{
		int id; // �ִϸ��̼� ������ ��ƲŬ���̾�Ʈ

		va_list ap;
		va_start(ap, argsCount);
		id = va_arg(ap, int);
		va_end(ap);


		/* ��ɵ��� ������Ʈ�� ������ ó���ؾ��Ѵ�.*/
		m_IsPlayerDieEventHandler = true;
		m_DiedPlayerIDEventParams.push(id);

		break;
	}
	case EVENT_GAMEPLAY_TIMER:
	{
		int timer; // �ִϸ��̼� ������ ��ƲŬ���̾�Ʈ

		va_list ap;
		va_start(ap, argsCount);
		timer = va_arg(ap, int);
		va_end(ap);

		m_Timer = timer;

		break;
	}
	case EVENT_GAMEPLAY_PENALTY_WARNING:
	{
		m_IsPenartyAlram = true;

		UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
		timer->InitSpriteAnimation();
		// �г�Ƽ ��� �˶� ����
		cout << "penalty warning start alram event" << endl;
		SoundManager::GetApp()->PlaySoundLoop(L"EFF_Player_Warning.mp3", SoundManager::CHANNEL_ID::PLAYER_PENALTY_WARNING);
		break;
	}

	case EVENT_GAMEPLAY_PENALTY_WARNING_END:
	{
		m_IsPenartyAlram = false;

		UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
		timer->InitSpriteAnimation();
		// �г�Ƽ ��� �˶� ��
		cout << "penalty warning end alram event" << endl;
		SoundManager::GetApp()->StopSound(SoundManager::CHANNEL_ID::PLAYER_PENALTY_WARNING);
		break;
	}

	case EVENT_GAMEPLAY_PENALTY:
	{
		int targetId;
		bool isPenarty;

		va_list ap;
		va_start(ap, argsCount);
		targetId = va_arg(ap, int);
		isPenarty = va_arg(ap, bool);
		va_end(ap);

		cout << targetId << " - isPenarty: " << isPenarty << endl;
		if (m_Users[targetId])
			m_Users[targetId]->PenartyParticle(isPenarty);

		if (targetId == m_PlayerID) {
			m_IsPenarty = isPenarty;
			// m_IsPenarty�� ���� Penalty sound on/off
			if (m_IsPenarty) {
				SoundManager::GetApp()->StopSound(SoundManager::CHANNEL_ID::PLAYER_PENALTY_WARNING);
				SoundManager::GetApp()->PlaySoundLoop(L"EFF_Player_Penalty.mp3", SoundManager::CHANNEL_ID::PLAYER_PENALTY);
			}
			else
				SoundManager::GetApp()->StopSound(SoundManager::CHANNEL_ID::PLAYER_PENALTY);

			m_IsPenartyAlram = false;

			UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
			timer->InitSpriteAnimation();
		}

		break;
	}
	case EVENT_GAMEPLAY_GAMEOVER:
	{
		// ����ȯ
		m_IsGameOver = true;
		break;
	}
	}
}

void GameplayScene::Initialize()
{
	/*Scn-Controller*/
	m_SceneController = new GameplayController(this);
	m_SceneController->SetMapName(MAP_STR_TOWN);

	/* SkyCube */
	AppContext->CreateSkycube("gameplaySky", "gameplaySky", "grasscube1024");

	/* ���� ������Ʈ�� ���� */
	AppContext->CreateProps(MAP_STR_TOWN);
	AppContext->CreateContourProps(MAP_STR_TOWN);

	/* �Ѿ� */
	AppContext->CreateThunderBolt();

	// �����Ʈ ��ƼŬ - Thunder
	for (int i = 0; i < MAX_THUNDERBOLT_COUNT; ++i)
	{
		AppContext->CreateParticle(PARTICLE_NAME_THUNDER, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + i), TEXTURE_STR_Thunderbolt, true, XMFLOAT3(0, 0, 0), 3.f);
		AppContext->CreateParticle(PARTICLE_NAME_LIGHT_EXPLOSION, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + i), TEXTURE_STR_P_LIGHT_EXPLOSION, false, XMFLOAT3(0, 0, 0), 5.f);
	}
	// ������ �����Ʈ �����͸� ��ƼŬ�� ��������
	ParticleSystem::GetApp()->SetThunderboltParticle(OBJECT_TYPE_THUNDERBOLT, OBJECT_START_INDEX_THUNDERBOLT, PARTICLE_NAME_THUNDER);
	ParticleSystem::GetApp()->SetThunderboltParticle(OBJECT_TYPE_THUNDERBOLT, OBJECT_START_INDEX_THUNDERBOLT, PARTICLE_NAME_LIGHT_EXPLOSION);

	/* HP  & Potion & BLUR - CRUSH MIRROR */
	AppContext->CreateUI2D(OBJECT_NAME_HP, OBJECT_NAME_HP, TEXTURE_INDEX_UI_HP);
	AppContext->CreateUI2D(OBJECT_NAME_PRO_POTION, OBJECT_NAME_PRO_POTION, TEXTURE_INDEX_UI_PRO_POTION);
	AppContext->CreateUI2D(OBJECT_NAME_CRUSH_MIRROR, OBJECT_NAME_CRUSH_MIRROR, TEXTURE_INDEX_CRUSH_MIRROR); // 

	/* ���� & Profile UI */
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_AIM, TEXTURE_INDEX_AIM);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_BACK, TEXTURE_INDEX_UI_PRO_BACK);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_BACK_MASTER, TEXTURE_INDEX_PRO_BACK_MASTER);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_PICTURE, TEXTURE_INDEX_UI_PRO_MASTER);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER, TEXTURE_INDEX_UI_TIME_0);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIME_BACKGROUND, TEXTURE_INDEX_TIME_BACKGROUND);

	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_BACK, TEXTURE_INDEX_UI_PLAYBOARD_BACKGROUND);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_DU, TEXTURE_INDEX_UI_PLAYBOARD_DRU, TEXTURE_INDEX_UI_PLAYBOARD_DRU_D);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_FEA, TEXTURE_INDEX_UI_PLAYBOARD_FEA, TEXTURE_INDEX_UI_PLAYBOARD_FEA_D);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_KING, TEXTURE_INDEX_UI_PLAYBOARD_KING, TEXTURE_INDEX_UI_PLAYBOARD_KING_D);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_LEE, TEXTURE_INDEX_UI_PLAYBOARD_LEE, TEXTURE_INDEX_UI_PLAYBOARD_LEE_D);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_MASTER, TEXTURE_INDEX_UI_PLAYBOARD_MASTER);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_RED, TEXTURE_INDEX_UI_PLAYBOARD_RED, TEXTURE_INDEX_UI_PLAYBOARD_RED_D);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_1, TEXTURE_INDEX_UI_PLAYBOARD_SCROLL);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_2, TEXTURE_INDEX_UI_PLAYBOARD_SCROLL);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_3, TEXTURE_INDEX_UI_PLAYBOARD_SCROLL);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_4, TEXTURE_INDEX_UI_PLAYBOARD_SCROLL);


	/* Set Timer Sprite Animation */
	UserInterface* penaltyTimer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
	penaltyTimer->SetSpriteAniamtion(SPRITE_NAME_TIMER_PENALTY, 15.f);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_8);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_7);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_6);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_5);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_4);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_3);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_2);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_1);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY, TEXTURE_INDEX_UI_TIME_0);

	penaltyTimer->SetSpriteAniamtion(SPRITE_NAME_TIMER_PENALTY_ALRAM, 1.f);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_10);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_9);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_8);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_7);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_6);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_5);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_4);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_3);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_2);
	penaltyTimer->AddFrame(SPRITE_NAME_TIMER_PENALTY_ALRAM, TEXTURE_INDEX_UI_TIME_ALARM_1);

	/* Param */
	m_IsArriveMyPlayer = true;
	m_IndexSaveForPlayerID = 1;
	m_IndexChangeOtherPlayerCamera = 1;
	m_PenaltyDuration = 0;
}

void GameplayScene::OnResize()
{
	GraphicsContext::GetApp()->OnResizeBlur();
}

bool GameplayScene::Enter()
{
	cout << "============= Gameplay Scene ==============" << endl;

	// �����÷��� ������ ���콺 ���̰� �ϰ�ʹٸ� �ּ�ó��
	InputHandler::g_CursorSwitch = false;
	m_IsArriveMyPlayer = true;

	/* Create SceneBounds for Shadow */
	m_SceneBounds.Center = XMFLOAT3(2500.f, 0.0f, 2500.f);
	m_SceneBounds.Radius = 2700.f;

	/* Light Setting */
	TOY_GROUND::GetApp()->m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 0.57735f, -0.81735f, -1.07735 };

#ifdef DEBUG_CLIENT
	// Player Setting
	// Props Setting
	m_PlayerID = 0;
	m_MapName = MAP_STR_TOWN;
	AppContext->DisplayProps(m_MapName);

	m_Users[m_PlayerID] = AppContext->FindObject<Character>(CHARACTER_WIZARD, CHARACTER_WIZARD);
	m_Users[m_PlayerID]->m_IsVisible = true;
	m_Users[m_PlayerID]->m_IsVisibleOnePassCheck = true;
	m_Users[m_PlayerID]->m_PlayerRole = ROLE_MASTER;

	m_Users[1] = AppContext->FindObject<Character>(CHARACTER_BAIRD, CHARACTER_BAIRD);
	m_Users[1]->m_IsVisible = true;
	m_Users[1]->m_IsVisibleOnePassCheck = true;
	m_Users[2] = AppContext->FindObject<Character>(CHARACTER_DRUID, CHARACTER_DRUID);
	m_Users[2]->m_IsVisible = true;
	m_Users[2]->m_IsVisibleOnePassCheck = true;
	m_Users[3] = AppContext->FindObject<Character>(CHARACTER_SORCERER, CHARACTER_SORCERER);
	m_Users[3]->m_IsVisible = true;
	m_Users[3]->m_IsVisibleOnePassCheck = true;


#elif DEBUG_SERVER
	/*Game Info Reset*/
	// �� ��Ʋ�������̵� �ҷ�����
	m_PlayerID = Service::GetApp()->GetMyBattleID();

	// Set Penalty Duration
	m_PenaltyDuration = Service::GetApp()->GetPenaltyDuration();
	UserInterface* penaltyTimer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
	if (penaltyTimer) penaltyTimer->SetDurationTime(SPRITE_NAME_TIMER_PENALTY, static_cast<float>(m_PenaltyDuration));

	// �� ���� �ҷ�����
	int mapType = Service::GetApp()->GetMyMapInfo();
	m_MapName = AppContext->FindMapName(mapType);
	AppContext->DisplayProps(m_MapName);

	// Ŭ���̾�Ʈ �÷��̾� ���� �ҷ�����
	int bcCount = Service::GetApp()->GetBattleClientsCount();
	m_PlayerIDs.resize(bcCount);
	Service::GetApp()->Notify(EVENT_GAMEPLAY_START);

	// ĳ���� ����
	for (auto& p : m_Users)
	{
		if (p.second)
		{
			AppContext->DisplayCharacter(m_MapName, p.second, p.second->m_SpawnLoaction);
			// �ٸ� Ŭ���̾�Ʈ ĳ���͵�, ������ Ŭ���̾�Ʈ�� �������� �˰��ֱ� ����
			std::string userClientName;
			if (m_Users[m_PlayerID]) userClientName = m_Users[m_PlayerID]->GetMeshName();
			p.second->m_UserPlayerMeshName = userClientName;
		}
	}

	// Profile
	if (m_Users[m_PlayerID]) {
		SetUI2DProfilePicture(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_PICTURE, m_Users[m_PlayerID]->GetMeshName());
	}
	else
	{
		cout << "SetUI2DProfilePicture Error: OBJECT_NAME_PRO_PICTURE - m_Users nullptr" << endl;
	}

#endif
	// Player type, id ��� ����
	// m_Users[]�� SetMapName ȣ���Ͽ� ĳ���Ͱ� �����ִ� ���̸� ����
	m_Users[m_PlayerID]->SetCamera(CACHE_CACHE::GetApp()->m_Camera, CameraType::Third);
	m_Users[m_PlayerID]->SetController(m_MapName);
	m_Users[m_PlayerID]->SetMapName(m_MapName);
	m_Users[m_PlayerID]->SetBlurLifeTime(0.5f);

	/* UI Setting */
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_AIM, XMFLOAT2(0.f, 0.f), XMFLOAT2(500.f, 500.f), false, AnchorType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIME_BACKGROUND, XMFLOAT2(0.f, 458.f), XMFLOAT2(3700.f, 1300.f), false, AnchorType::Center);

	/* Profile HUD UI */
	if (m_Users[m_PlayerID])
	{
		if (m_Users[m_PlayerID]->m_PlayerRole == ROLE_MASTER)
		{
			AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_BACK_MASTER, XMFLOAT2(-725.f, -350.f), XMFLOAT2(5000.f, 2600.f), false, AnchorType::LT);
		}
		else
		{
			/* HP & Potion */
			AppContext->DisplayUI2D(OBJECT_NAME_PRO_POTION, OBJECT_NAME_PRO_POTION, XMFLOAT2(-725.f, -350.f), XMFLOAT2(5000.f, 2600.f), false, AnchorType::LT);
			AppContext->DisplayUI2D(OBJECT_NAME_HP, OBJECT_NAME_HP, XMFLOAT2(-725.f, -350.f), XMFLOAT2(5000.f, 2600.f), false, AnchorType::LT);
			AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_BACK, XMFLOAT2(-725.f, -350.f), XMFLOAT2(5000.f, 2600.f), false, AnchorType::LT);
			AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER, XMFLOAT2(-725.f, -350.f), XMFLOAT2(5000.f, 2600.f), false, AnchorType::LT);
		}
	}
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_PICTURE, XMFLOAT2(-725.f, -350.f), XMFLOAT2(5000.f, 2600.f), false, AnchorType::LT);

	/* Thunderbolt ��ġ �ʱ�ȭ �� ������Ʈ ����*/
	for (int i = 0; i < MAX_THUNDERBOLT_COUNT; ++i)
	{
		AppContext->HiddenThunderBolt(i);
		AppContext->HiddenParticle(PARTICLE_NAME_THUNDER, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + i));
	}

#ifdef DEBUG_SERVER
	// �÷��̾� ���� ����
	m_PlayerBoardInfo.reserve(4);
	m_PlayerBoardInfo.emplace_back(BoardInfo{ DirectX::XMFLOAT2(-507.f, 10.f), L"" });
	m_PlayerBoardInfo.emplace_back(BoardInfo{ DirectX::XMFLOAT2(-163.f, 10.f), L"" });
	m_PlayerBoardInfo.emplace_back(BoardInfo{ DirectX::XMFLOAT2(172.f, 10.f) , L"" });
	m_PlayerBoardInfo.emplace_back(BoardInfo{ DirectX::XMFLOAT2(502.5f, 10.f), L"" });

	for (auto& p : m_Users)
	{
		if (p.second && p.second->m_PlayerRole == ROLE_MASTER)
		{
			BattleClient* battleInfo = Service::GetApp()->GetBattleClient(p.second->m_BattleServerID);
			if (battleInfo)
				m_PlayerBoardInfo[0].id = battleInfo->GetName();
			break;
		}
	}

	/* Set PlayBoard */
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_BACK, XMFLOAT2(0.f, 0.f), XMFLOAT2(14000.f, 4000.f), false, AnchorType::Center);

	// Master PlayBoard
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_MASTER, XMFLOAT2(-506.f, 10.f), XMFLOAT2(2950.f, 2950.f), false, AnchorType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_1,
		XMFLOAT2(m_PlayerBoardInfo[0].pos.x, m_PlayerBoardInfo[0].pos.y - 145.f), XMFLOAT2(3390.f, 997.f), false, AnchorType::Center, UI_LAYER_BACK, true);

	int cnt = 1;
	for (auto& p : m_Users)
	{
		if (p.second && p.second->m_PlayerRole == ROLE_STUDENT)
		{
			std::string userClientName;
			userClientName = p.second->GetMeshName();

			BattleClient* battleInfo = Service::GetApp()->GetBattleClient(p.second->m_BattleServerID);
			m_PlayerBoardInfo[cnt].id = battleInfo->GetName();

			// PlayBoard Charactor Image ����
			std::string playBoardImageName;
			playBoardImageName = ConvertCharacterNameToPlayboardImageName(userClientName);

			AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, playBoardImageName, m_PlayerBoardInfo[cnt].pos, XMFLOAT2(2950.f, 2950.f), false, AnchorType::Center);
			AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, playBoardImageName)->Overlap(false);

			// PlayBoard Scroll ����
			AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, "PLAYBOARD_Z_SCROLL_" + to_string(cnt + 1),
				XMFLOAT2(m_PlayerBoardInfo[(cnt)].pos.x, m_PlayerBoardInfo[(cnt)].pos.y - 145.f), XMFLOAT2(3390.f, 997.f), false, AnchorType::Center, UI_LAYER_BACK, true);
			++cnt;
		}
	}
#endif

	// ī�޶� ����
	CACHE_CACHE::GetApp()->m_Camera->CameraInitialize(SceneType::GamePlay);

	//  �� ��� ���� ����
	if (m_MapName == MAP_STR_TOWN)
		SoundManager::GetApp()->PlayBGM(L"BGM_Town(Low).wav", 0.8f);

	return false;
}

void GameplayScene::Exit()
{
	//  �� ��� ���� ����
	SoundManager::GetApp()->StopAll();

	//CommandCenter Reset
	CommandCenter::GetApp()->Release();

	// Init Gameplay Scene Parameters
	m_Users.clear();
	m_PlayerIDs.clear();
	m_PlayerNames.clear();
	m_PlayerBoardInfo.clear();

	// ��� ������Ʈ �������� (HiddenCharacter���� ������)
	// ������Ʈ �ʱⰪ���� ����
	AppContext->HiddenProps(m_MapName);
	AppContext->HiddenCharacter(CHARACTER_WIZARD);
	AppContext->HiddenCharacter(CHARACTER_DRUID);
	AppContext->HiddenCharacter(CHARACTER_BAIRD);
	AppContext->HiddenCharacter(CHARACTER_FEMALE_PEASANT);
	AppContext->HiddenCharacter(CHARACTER_MALE_PEASANT);
	AppContext->HiddenCharacter(CHARACTER_SORCERER);

	// ��ġ ����
	AppContext->HiddenParticle(PARTICLE_NAME_SMOKE, CHARACTER_DRUID);
	AppContext->HiddenParticle(PARTICLE_NAME_SMOKE, CHARACTER_BAIRD);
	AppContext->HiddenParticle(PARTICLE_NAME_SMOKE, CHARACTER_FEMALE_PEASANT);
	AppContext->HiddenParticle(PARTICLE_NAME_SMOKE, CHARACTER_MALE_PEASANT);
	AppContext->HiddenParticle(PARTICLE_NAME_SMOKE, CHARACTER_SORCERER);

	AppContext->HiddenParticle(PARTICLE_NAME_PENALTY, CHARACTER_DRUID);
	AppContext->HiddenParticle(PARTICLE_NAME_PENALTY, CHARACTER_BAIRD);
	AppContext->HiddenParticle(PARTICLE_NAME_PENALTY, CHARACTER_FEMALE_PEASANT);
	AppContext->HiddenParticle(PARTICLE_NAME_PENALTY, CHARACTER_MALE_PEASANT);
	AppContext->HiddenParticle(PARTICLE_NAME_PENALTY, CHARACTER_SORCERER);

	for (int i = 0; i < MAX_THUNDERBOLT_COUNT; ++i)
	{
		AppContext->HiddenThunderBolt(i, true);
		AppContext->HiddenParticle(PARTICLE_NAME_THUNDER, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + i));
		AppContext->HiddenParticle(PARTICLE_NAME_LIGHT_EXPLOSION, std::to_string(OBJECT_START_INDEX_THUNDERBOLT + i));
		// thunder particle hidden
		// light explore particle hidden
	}

	// HiddenUI2Ds
	AppContext->HiddenUI2D(OBJECT_NAME_HP, OBJECT_NAME_HP);
	AppContext->HiddenUI2D(OBJECT_NAME_PRO_POTION, OBJECT_NAME_PRO_POTION);

	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_BACK);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_DU);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_FEA);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_KING);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_LEE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_MASTER);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_RED);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_1);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_2);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_3);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, OBJECT_NAME_PLAYBOARD_SCROLL_4);

	/* Profile UI */
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_AIM);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_BACK);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_BACK_MASTER);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_PRO_PICTURE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);

	cout << "===========================================" << endl << endl;
}

void GameplayScene::Update(const float& fDeltaTime)
{
	// ������Ʈ �ϱ��� ����ó��
	if (m_MapName == "")
	{
		int mapType = Service::GetApp()->GetMyMapInfo();
		m_MapName = AppContext->FindMapName(mapType);

		if (m_MapName == "") cout << "���� ��ƲŬ���̾�Ʈ�� MapName�� �������� �ʽ��ϴ�." << endl;
	}

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

	/*Props Contour*/
	if (m_Users[m_PlayerID])
	{
		if (m_Users[m_PlayerID]->m_ContourObj.objectName != "")
		{
			auto origin = AppContext->m_RItemsVec[m_Users[m_PlayerID]->m_ContourObj.instanceID];
			GraphicsContext::GetApp()->UpdateContour(AppContext->m_RItemsMap[m_Users[m_PlayerID]->m_ContourObj.objectName + "Contour"], AppContext->m_RItemsVec, m_Users[m_PlayerID]->m_ContourObj.instName, origin);
		}
	}

	/*ThunderBolt*/
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[std::to_string(OBJECT_TYPE_THUNDERBOLT)], AppContext->m_RItemsVec);

	/*Characters*/
	// ���߿� ������ �޽��� ������Ʈ �ϴ°ŷ� �����ҿ���
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_WIZARD], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_DRUID], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_BAIRD], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_FEMALE_PEASANT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_MALE_PEASANT], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[CHARACTER_SORCERER], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Wizard, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_WIZARD].get());
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Druid, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_DRUID].get());
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Baird, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_BAIRD].get());
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Female_Peasant, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_FEMALE_PEASANT].get());
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Male_Peasant, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_MALE_PEASANT].get());
	GraphicsContext::GetApp()->UpdateSkinnedCBs(BoneIndex::Sorcerer, AssertsReference::GetApp()->m_SkinnedModelInsts[CHARACTER_SORCERER].get());

	/* UI */
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);


	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName], AppContext->m_RItemsVec);


	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_CRUSH_MIRROR], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_CRUSH_MIRROR], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_PRO_POTION], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_PRO_POTION], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_HP], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_HP], AppContext->m_RItemsVec);

	if (m_Users[m_PlayerID]) { GraphicsContext::GetApp()->UpdateUIPassCB(m_Users[m_PlayerID]->m_HP); }
	else {
		GraphicsContext::GetApp()->UpdateUIPassCB(0);
		//cout << "m_Users[m_PlayerID] ������ �ź� �߻� -> ���� ������ ����ȭ ����" << endl;
	}

	/*Particle*/
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_SMOKE], AppContext->m_RItemsVec, false, true);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_PENALTY], AppContext->m_RItemsVec, false, true);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_THUNDER], AppContext->m_RItemsVec, false, true);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[PARTICLE_NAME_LIGHT_EXPLOSION], AppContext->m_RItemsVec, false, true);

	/*Shadow*/
	GraphicsContext::GetApp()->UpdateShadowTransform(CACHE_CACHE::GetApp()->m_Lights[LIGHT_NAME_DIRECTIONAL].get(), m_SceneBounds);
	GraphicsContext::GetApp()->UpdateShadowPassCB();

	/*Materials*/
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void GameplayScene::Render()
{
	// ���� �ϱ��� ����ó��
	if (m_MapName == "")
	{
		int mapType = Service::GetApp()->GetMyMapInfo();
		m_MapName = AppContext->FindMapName(mapType);

		if (m_MapName == "") cout << "���� ��ƲŬ���̾�Ʈ�� MapName�� �������� �ʽ��ϴ�." << endl;
	}

	//
	// Main rendering pass.
	//
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());
	/*Props*/
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	/*ThunderBolt*/
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[std::to_string(OBJECT_TYPE_THUNDERBOLT)], AppContext->m_RItemsVec);

	/*Props contour*/
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_ContourPSO.Get());
	if (m_Users[m_PlayerID])
	{
		if (m_Users[m_PlayerID]->m_ContourObj.objectName != "")
		{
			// cout << m_PlayerID << "- GamePlay: " << m_Users[m_PlayerID]->m_ContourObj.objectName << ", " << m_Users[m_PlayerID]->m_ContourObj.instanceID << endl;
			GraphicsContext::GetApp()->DrawRenderContour(AppContext->m_RItemsMap[m_Users[m_PlayerID]->m_ContourObj.objectName + "Contour"], AppContext->m_RItemsVec, m_Users[m_PlayerID]->m_ContourObj.instName);
		}
	}

	/*Characters*/
	for (auto& p : m_Users)
	{
		if (!p.second) continue;

		if (p.second->m_TransformType == TransformType::Character)
			GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedPSO.Get());
		else
			GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}

	/*SkyBox*/
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkyPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap["gameplaySky"], AppContext->m_RItemsVec);

	/*Particle*/
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_ParticlePSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_SMOKE], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_PENALTY], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_THUNDER], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[PARTICLE_NAME_LIGHT_EXPLOSION], AppContext->m_RItemsVec);

	/* UI */
	if (!m_DebugNoUI) {
		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_CRUSH_MIRROR], AppContext->m_RItemsVec);
		if (m_IsTab)
			GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName], AppContext->m_RItemsVec);

		// HP
		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_HPPSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_HP], AppContext->m_RItemsVec);

		// Portion
		GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_PRO_POTION], AppContext->m_RItemsVec);
	}

	ThreadEventHandler();
}

void GameplayScene::RenderUI()
{
	if (m_DebugNoUI) return;


	// Timer
	GraphicsContext::GetApp()->SetTextSize(Core::g_DisplayWidth / 20);
	GraphicsContext::GetApp()->DrawD2DText(std::to_wstring(m_Timer / 60) + L" : " + std::to_wstring(m_Timer % 60), 0, -Core::g_DisplayHeight / 2.35, Core::g_DisplayWidth);

	if (m_IsTab)
	{
		// master
		UITextInfo info = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_PLAYBOARD_SCROLL_1);
		GraphicsContext::GetApp()->SetTextSize(info.size.y / 30.f);
		float defaultPosX = info.pos.x - (info.size.x / 20.f);
		GraphicsContext::GetApp()->DrawD2DText(m_PlayerBoardInfo[0].id, defaultPosX + (info.size.x / 20.f), -info.pos.y - (info.size.y / 20.f) * 0.3f, Core::g_DisplayWidth);

		// student
		string scroll = "PLAYBOARD_Z_SCROLL_";
		for (int i = 1; i < m_PlayerBoardInfo.size(); ++i)
		{
			UITextInfo info = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName], AppContext->m_RItemsVec, scroll + to_string(i + 1));
			GraphicsContext::GetApp()->SetTextSize(info.size.y / 30.f);
			float defaultPosX = info.pos.x - (info.size.x / 20.f);

			GraphicsContext::GetApp()->DrawD2DText(m_PlayerBoardInfo[i].id, defaultPosX + (info.size.x / 20.f), -info.pos.y - (info.size.y / 20.f) * 0.3f, Core::g_DisplayWidth);
		}
	}
}

void GameplayScene::WriteShadow()
{
	//
	// Shadow map pass.
	//
	GraphicsContext::GetApp()->SetResourceShadowPassCB();
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_ShadowOpaquePSO.Get());

	/*Shadow Props*/
	for (std::string prop : AppContext->m_Maps[m_MapName]->propTypeVector)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[prop], AppContext->m_RItemsVec);
	}

	/*Shadow ThunderBolt*/
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[std::to_string(OBJECT_TYPE_THUNDERBOLT)], AppContext->m_RItemsVec, true);

	/*Shadow Characters*/
	for (auto& p : m_Users)
	{
		if (!p.second) continue;

		if (p.second->m_TransformType == TransformType::Character)
			GraphicsContext::GetApp()->SetPipelineState(Graphics::g_SkinnedShadowOpaquePSO.Get());
		else
			GraphicsContext::GetApp()->SetPipelineState(Graphics::g_ShadowOpaquePSO.Get());
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[p.second->GetMeshName()], AppContext->m_RItemsVec);
	}

	GraphicsContext::GetApp()->ShadowTransitionResourceBarrier();
}

void GameplayScene::ThreadEventHandler()
{
	/* �̺�Ʈ ����ȭ �۾� */
	// �г�Ƽ �˶� ���� ��Ŷ�� �����ֱ�� ��.
	if (m_IsPenartyAlram)
	{
		UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
		timer->PlaySpriteAnimation(GameTimer::GetApp()->DeltaTime(), SPRITE_NAME_TIMER_PENALTY_ALRAM, true);
	}

	if (m_IsPenarty)
	{
		UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
		timer->PlaySpriteAnimation(GameTimer::GetApp()->DeltaTime(), SPRITE_NAME_TIMER_PENALTY, false);
		//
	}

	// Die �̺�Ʈ ����ȭ �۾�
	if (m_IsPlayerDieEventHandler)
	{
		// Die �۾��ϱ�
		DiePlayer();

		if (m_DiedPlayerIDEventParams.empty())
			m_IsPlayerDieEventHandler = false;
	}

	if (m_IsGameOver)
	{
		// ���Ӱ�������� ���� �÷��̾� �޽� ���� ������
		for (auto& p : m_PlayerNames)
		{
			Service::GetApp()->Notify(EVENT_LOADING_GAMERESULT_INFO, 1, p);
		}
		m_PlayerNames.clear();
		SceneManager::GetApp()->ChangeScene();
		m_IsGameOver = false;
	}
}

std::string GameplayScene::ConvertCharacterNameToPlayboardImageName(std::string chrName)
{
	std::string playBoardImageName;

	if (chrName == CHARACTER_BAIRD)
		playBoardImageName = OBJECT_NAME_PLAYBOARD_FEA;
	else if (chrName == CHARACTER_DRUID)
		playBoardImageName = OBJECT_NAME_PLAYBOARD_DU;
	else if (chrName == CHARACTER_FEMALE_PEASANT)
		playBoardImageName = OBJECT_NAME_PLAYBOARD_RED;
	else if (chrName == CHARACTER_MALE_PEASANT)
		playBoardImageName = OBJECT_NAME_PLAYBOARD_LEE;
	else if (chrName == CHARACTER_SORCERER)
		playBoardImageName = OBJECT_NAME_PLAYBOARD_KING;

	return playBoardImageName;
}

void GameplayScene::SetUI2DProfilePicture(std::string ui2dName, std::string ui2dInstId, std::string playerName)
{
	UserInterface* item = AppContext->FindObject<UserInterface>(ui2dName, ui2dInstId);
	if (!item) {
		cout << "SetUI2DProfilePicture: Cant Find Object" << endl;
		return;
	}

	if (playerName == CHARACTER_BAIRD)
		item->m_MaterialIndex = TEXTURE_INDEX_UI_PRO_BA;
	else if (playerName == CHARACTER_DRUID)
		item->m_MaterialIndex = TEXTURE_INDEX_UI_PRO_DU;
	else if (playerName == CHARACTER_FEMALE_PEASANT)
		item->m_MaterialIndex = TEXTURE_INDEX_UI_PRO_FE;
	else if (playerName == CHARACTER_MALE_PEASANT)
		item->m_MaterialIndex = TEXTURE_INDEX_UI_PRO_MA;
	else if (playerName == CHARACTER_SORCERER)
		item->m_MaterialIndex = TEXTURE_INDEX_UI_PRO_SO;
	else if (playerName == CHARACTER_WIZARD)
		item->m_MaterialIndex = TEXTURE_INDEX_UI_PRO_MASTER;
}

void GameplayScene::ChangeCameraNextPlayer()
{
	if (m_IsArriveMyPlayer) return;

	m_IndexChangeOtherPlayerCamera += 1;
	if (m_IndexChangeOtherPlayerCamera >= m_PlayerIDs.size())
	{
		m_IndexChangeOtherPlayerCamera = 0;
	}

	if (!m_Users[m_PlayerIDs[m_IndexChangeOtherPlayerCamera]]) return;

	// �ٸ� ĳ���͵� ī�޶� ������ ����
	// ī�޶�� �� �ϳ��� �����ؾ���.
	for (auto& p : m_Users)
	{
		if (p.second) p.second->m_MyCamera = nullptr;
	}

	std::string changeCameraPlayerMeshName = m_Users[m_PlayerIDs[m_IndexChangeOtherPlayerCamera]]->GetMeshName();
	AppContext->FindObject<Character>(changeCameraPlayerMeshName, changeCameraPlayerMeshName)->SetCamera(CACHE_CACHE::GetApp()->m_Camera, CameraType::Third);
}

void GameplayScene::ChangeFreeCamera()
{
	// if (CACHE_CACHE::GetApp()->m_Camera->GetCameraType() == CameraType::Free) return;
	if (!m_Users[m_PlayerIDs[m_IndexChangeOtherPlayerCamera]]) return;


	XMFLOAT3 look = m_Users[m_PlayerIDs[m_IndexChangeOtherPlayerCamera]]->GetLook();
	XMFLOAT3 up = m_Users[m_PlayerIDs[m_IndexChangeOtherPlayerCamera]]->GetUp();
	XMFLOAT3 right = m_Users[m_PlayerIDs[m_IndexChangeOtherPlayerCamera]]->GetRight();

	CACHE_CACHE::GetApp()->m_Camera->SetCamera(look, up, right);
}

void GameplayScene::DiePlayer()
{
	int id = m_DiedPlayerIDEventParams.front();

	// ����ó��
	if (!m_Users[id]) return;

	// playboard ������� �ؽ��� �������ֱ�
	std::string userClientName = m_Users[id]->GetMeshName();
	std::string playBoardImageName;
	playBoardImageName = ConvertCharacterNameToPlayboardImageName(userClientName);
	AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D_PLAYBOARD + m_SceneName, playBoardImageName)->Overlap(true);

	// �״� ���� �߰�
	SoundManager::GetApp()->PlaySoundOnce(L"EFF_Player_Transform.wav", SoundManager::CHANNEL_ID::PLAYER_TRANSFORM, 0.6f);

	// ������� ����Ʈ ȿ��
	m_Users[id]->TransformParticle(true);

	// ��������Ʈ �ִϸ��̼� �ʱ�ȭ
	m_IsPenarty = false;
	m_IsPenartyAlram = false;
	UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
	timer->InitSpriteAnimation();

	// ĳ���� �����
	AppContext->HiddenCharacter(m_Users[id]->GetMeshName());
	m_Users.erase(id);
	m_PlayerIDs.erase(remove(m_PlayerIDs.begin(), m_PlayerIDs.end(), id), m_PlayerIDs.end());

	// ���� �׾����ÿ�
	if (id == m_PlayerID) {
		m_IsArriveMyPlayer = false;
		// ���� �����
		AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_AIM);

		// �г�Ƽ �� ������ ��������Ʈ �ִϸ��̼ǰ� ���� �ʱ�ȭ
		m_IsPenartyAlram = false;
		m_IsPenarty = false;

		SoundManager::GetApp()->StopSound(SoundManager::CHANNEL_ID::PLAYER_PENALTY_WARNING);
		SoundManager::GetApp()->StopSound(SoundManager::CHANNEL_ID::PLAYER_PENALTY);

		UserInterface* timer = AppContext->FindObject<UserInterface>(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_TIMER);
		timer->InitSpriteAnimation();

		// ���ӿ����� �ƴ϶�� ī�޶� �������� �̵�
		AppContext->FindObject<Character>(CHARACTER_WIZARD, CHARACTER_WIZARD)->SetCamera(CACHE_CACHE::GetApp()->m_Camera, CameraType::Third);
	}

	m_DiedPlayerIDEventParams.pop();
}

