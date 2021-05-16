#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Service.h"

#include "LobbyScene.h"
#include "GameplayScene.h"

void SceneManager::SendEventArgs(SceneType st, int sEvent, int argsCount, ...) {
#ifdef LOG_ON
	if (st == SceneType::eGamePlay) {
		cout << "[GAME] event: " << (int)sEvent << endl;
	}
	if (st == SceneType::eLobby) {
		cout << "[LOBBY] event: " << (int)sEvent << endl;
	}
#endif
	if (argsCount == 0) {
		//cout << "[SceneMnr] event: " << (int)sEvent << endl;
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent);
		return;
	}

	switch (sEvent) { //break 바로 들어가는건 argsCount가 0이라 위에서 처리함
		//lobby
	case EVENT_LOBBY_LOGIN_OK: break;
	case EVENT_LOBBY_LOGIN_FAIL: break;
	case EVENT_LOBBY_SIGNUP_OK: break;
	case EVENT_LOBBY_SIGNUP_FAIL: break;
	case EVENT_LOBBY_UPDATE_CLIENT_USERINFO: {
		va_list ap;
		int mmr;
		va_start(ap, argsCount);
		mmr = va_arg(ap, int);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, mmr);
		break;
	}
		//battle
	case EVENT_BATTLE_LOGIN_OK: break;
	case EVENT_ROOM_ENTER: {
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_id);
		break;
	}
	case EVENT_ROOM_LEAVE: {
		int leaver{};
		va_list ap;
		va_start(ap, argsCount);
		leaver = va_arg(ap, int);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, leaver);
		break;
	}
	case EVENT_ROOM_READY: {
		int id;
		bool ready;
		va_list ap;
		va_start(ap, argsCount);
		id = va_arg(ap, int);
		ready = va_arg(ap, bool);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, id, ready);
		break;
	}
	case EVENT_ROOM_START_AVAILABLE: { //일단 바로 시작하는걸로
		bool isAvailable{};
		va_list ap;
		va_start(ap, argsCount);
		isAvailable = va_arg(ap, bool);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, isAvailable);
		break;
	}
	case EVENT_GAME_START: {
		//씬전환 eLobby -> eGamePlay
		int arg_bt_id;
		XMFLOAT4 arg_sl;

		va_list ap;
		va_start(ap, argsCount);
		arg_bt_id = va_arg(ap, int);
		arg_sl = va_arg(ap, XMFLOAT4);
		va_end(ap);

		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_bt_id, arg_sl);
		break;
	}
	//ingame
	case EVENT_GAME_TIMER: {
		int t;
		va_list ap;
		va_start(ap, argsCount);
		t = va_arg(ap, int);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, t);
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

		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_id, arg_anim_type);
		break;
	}
	case EVENT_GAME_ADD_COIN: {
		XMFLOAT3 arg_pos;
		va_list ap;
		va_start(ap, argsCount);
		arg_pos = va_arg(ap, XMFLOAT3);
		va_end(ap);
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_pos);
		break;
	}
	case EVENT_GAME_UPDATE_COIN: {
		int arg_id;
		int arg_coin_cnt;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_coin_cnt = va_arg(ap, int);
		va_end(ap);
		
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_id, arg_coin_cnt);
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
		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_id, arg_pos);
		break;
	}
	case EVENT_GAME_CALLBACK_MOUSE: {
		int arg_id;
		XMFLOAT3 arg_look{};
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		arg_look = va_arg(ap, XMFLOAT3);
		va_end(ap);

		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_id, arg_look);
		break;
	}
	case EVENT_GAME_GAMEOVER: {
		int arg_winner;
		va_list ap;
		va_start(ap, argsCount);
		arg_winner = va_arg(ap, int);
		va_end(ap);

		m_Scenes[static_cast<int>(st)]->ProcessEvent(sEvent, argsCount, arg_winner);
		break;
	}
	}
}

SceneManager::SceneManager() : m_CurScene(0)
{
}

SceneManager::~SceneManager()
{
	for (auto& s : m_Scenes) {
		SAFE_DELETE_PTR(s->m_SceneController);
	}

	for (auto& s : m_Scenes) {
		s->Exit();
		SAFE_DELETE_PTR(s);
	}
}

void SceneManager::InitializeScenes()
{
	CreateScene<LobbyScene>(SceneType::eLobby, "Lobby");
	CreateScene<GameplayScene>(SceneType::eGamePlay, "GamePlay");
}

void SceneManager::OnResize()
{
	m_Scenes[m_CurScene]->OnResize();
}

void SceneManager::ChangeScene()
{
	if (m_CurScene == -1)
		cout << "NullScene" << endl;

	m_MutexChangeScene.lock();
	m_Scenes[m_CurScene]->Exit();

	m_CurScene += 1;

	if (m_CurScene >= static_cast<int>(SceneType::eCount))
		m_CurScene = 1;

	m_Scenes[m_CurScene]->Enter();
	m_MutexChangeScene.unlock();
}

void SceneManager::ChangeScene(SceneType sceneType)
{
	if (m_CurScene == -1)
		cout << "NullScene" << endl;
	if (m_CurScene == (int)sceneType) return;
	m_MutexChangeScene.lock();
	m_Scenes[m_CurScene]->Exit();

	m_CurScene = static_cast<int>(sceneType);
	m_Scenes[m_CurScene]->Enter();
	m_MutexChangeScene.unlock();
}

void SceneManager::EnterScene(SceneType sceneType)
{
	m_CurScene = static_cast<UINT>(sceneType);
	m_Scenes[m_CurScene]->Enter();
}

void SceneManager::ExitScene()
{
	m_Scenes[m_CurScene]->Exit();
}

void SceneManager::UpdateScene(const float& deltaT)
{
	m_Scenes[m_CurScene]->Update(deltaT);
}

void SceneManager::RenderScene()
{
	m_Scenes[m_CurScene]->Render();
}

Scene* SceneManager::GetCurScene() const
{
	return m_Scenes[m_CurScene];
}

UINT SceneManager::GetCurSceneIndex() const
{
	return m_CurScene;
}

