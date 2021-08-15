#include "pch.h"
#include "Room.h"
//#define LOG_ON

Room::Room(int room_no) {
	Initialize(room_no);
}

Room::Room(int room_no, wstring room_name) {
	Initialize(room_no);
	m_roomName = room_name;
}

Room::~Room() {
	//if (m_physics != nullptr) {
	//	delete m_physics;
	//	m_physics = nullptr;
	//}
	//if (m_map != nullptr) {
	//	delete m_map;
	//	m_map = nullptr;
	//}

	for (auto& p : m_players) {
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}
	}
}

void Room::Initialize(int room_no) {
	m_roomNo = room_no;
	m_curPlayerNum = 0;
	m_isGameStarted = false;
	m_isRoundStarted = false;
	m_leftTime = MAX_LEFT_TIME;
	m_coin_cur = 0;
	//m_physics = new Physics();
	//m_map = new Map();
	m_lastUpdate = std::chrono::system_clock::now();

	for (auto& p : m_players) p = new Toy();

	for (auto& a : m_bullets) {
		a = false;
	}
	while (!m_q_bullet.empty()) {
		m_q_bullet.pop();
	}

	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30ms), EVENT_TYPE::EV_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev);
}

void Room::Reset() {
	if (this == nullptr) return;

	m_isEnterable = true;
	m_isRoundStarted = false;
	m_leftTime = MAX_LEFT_TIME;
	m_lastUpdate = std::chrono::system_clock::now();
	m_curPlayerNum = 0;

	//delete m_map;
	//m_map = new Map();

	for (auto& p : m_players) {
		static_cast<Toy*>(p)->Reset();
	}
}

void Room::Update() {
	if (this == nullptr) return;

	m_curUpdate = std::chrono::system_clock::now();
	m_elapsedSec = m_curUpdate - m_lastUpdate;
	m_lastUpdate = m_curUpdate;

	float elapsedMilliSec{ (m_elapsedSec.count() * 1000.f) };

	CopyRecvMsgs();
	message procMsg;
	while (!m_copiedRecvMsgs.empty()) {
		procMsg = m_copiedRecvMsgs.front();
		m_copiedRecvMsgs.pop();
		ProcMsg(procMsg);
	}

	if (m_isGameStarted) {
		AnimationUpdate(elapsedMilliSec);
		WorldUpdate();
		//Collision();
		//for (int i = 0; i < MAX_PLAYER; ++i) {
		//	if (m_players[i]->GetID() != -1) { //not unset
		//		if (m_players[i]->IsDead() == false) {
		//			auto iter = m_players[i]->GetCurObject();
		//			XMFLOAT3 curpos = iter->GetPosition();
		//			XMFLOAT3 prepos = iter->GetPrePosition();
		//			XMFLOAT3 subDistance;
		//			subDistance = SMathHelper::Subtract(curpos, prepos);
		//			if (fabs(subDistance.x) >= 0.1f
		//				|| fabs(subDistance.y) >= 3.f
		//				|| fabs(subDistance.z) >= 0.1f) {
		//				PTC_VECTOR ptc_pos;
		//				ptc_pos.x = curpos.x;
		//				ptc_pos.y = curpos.y;
		//				ptc_pos.z = curpos.z;
		//				iter->SetPrePosition(curpos);
		//				for (int j = 0; j < MAX_PLAYER; ++j) {
		//					if (m_players[j]->GetID() != -1) {
		//						PushPlayerPositionMsg(m_players[j]->GetID(), m_players[i]->GetID(), &ptc_pos);
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
		//for (auto iter = m_map->m_obj_list[Map::NOTFIXED].begin(); iter != m_map->m_obj_list[Map::NOTFIXED].end(); ++iter) {
		//	XMFLOAT3 curpos = (*iter)->GetPosition();
		//	XMFLOAT3 prepos = (*iter)->GetPrePosition();
		//
		//	subDistance = SMathHelper::Subtract(curpos, prepos);
		//
		//	if (fabs(subDistance.x) >= 0.1f
		//		|| fabs(subDistance.y) >= 1.f
		//		|| fabs(subDistance.z) >= 0.1f) { //움직였다면
		//		short type_id = (*iter)->m_boundaries->GetObjType();
		//		int obj_id = (*iter)->GetUniqueID();
		//		PTC_VECTOR ptc_pos;
		//
		//		ptc_pos.x = curpos.x;
		//		ptc_pos.y = curpos.y;
		//		ptc_pos.z = curpos.z;
		//
		//		for (int i = 0; i < MAX_PLAYER; ++i) {
		//			if (m_players[i]->GetID() != -1) { //not unset
		//				PushObjectPositionMsg(m_players[i]->GetID(), type_id, obj_id, &ptc_pos); //클라 obj 움직임이 끝난다면
		//			}
		//		}
		//	}
		//}
		//for (int i = 0; i < m_bullets.size(); ++i) {
		//	if (m_bullets[i].isShoot()) {
		//		XMFLOAT3 pos = m_bullets[i].GetPosition();
		//		PTC_VECTOR ptc_pos;
		//		ptc_pos.x = pos.x;
		//		ptc_pos.y = pos.y;
		//		ptc_pos.z = pos.z;
		//
		//		for (int j = 0; i < MAX_PLAYER; ++j) {
		//			if (m_players[j]->GetID() != -1) {
		//				PushObjectPositionMsg(m_players[j]->GetID(), m_bullets[i].m_boundaries->GetObjType(), m_bullets[i].GetUniqueID(), &ptc_pos);
		//			}
		//		}
		//	}
		//}
	}

	std::chrono::system_clock::time_point updateEnd{ std::chrono::system_clock::now() };
	std::chrono::duration<float> updateTime{ updateEnd - m_curUpdate };

	//60fps
	int nextUpdate{ static_cast<int>((0.016f - updateTime.count()) * 1000.0f) };
	if (nextUpdate <= 0) nextUpdate = 0;

	EVENT ev_update{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(nextUpdate), EVENT_TYPE::EV_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev_update);

	EVENT ev_flush{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1), EVENT_TYPE::EV_FLUSH_MSG };
	BattleServer::GetInstance()->AddTimer(ev_flush);
}

void Room::PushMsg(message& msg) {
	if (msg.id <= 0) return; // 0은 lobby

	ATOMIC::g_msg_lock.lock();
	m_recvMsgQueue.push(msg);
	ATOMIC::g_msg_lock.unlock();
}

void Room::AnimationUpdate(float elapsedMilliSec)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		int id{ m_players[i]->GetID() };
		if (id != -1) {//not unset
			m_players[i]->Update(elapsedMilliSec);
			int anim_type{ m_players[i]->GetAnimType() };
		}
	}
}

void Room::WorldUpdate() {
	//유동 object update -> bullet update
	//for (auto it = m_map->m_obj_list[Map::NOTFIXED].begin();
	//	it != m_map->m_obj_list[Map::NOTFIXED].end();
	//	++it) {
	//	(*it)->Update(m_elapsedSec.count() * 1000.f, false);
	//}
	//
	//for (int i = 0; i < m_bullets.size(); ++i) {
	//	if (m_bullets[i].isShoot()) {
	//		m_bullets[i].BulletUpdate(m_elapsedSec.count() * 1000.f);
	//		if (!m_bullets[i].isBulletActive()) {
	//			m_bullets[i].SetShoot(false);
	//			
	//		}
	//	}
	//}
}

bool Room::IsEmpty() {
	if (m_curPlayerNum <= 0) return true;
	else return false;
}

bool Room::EnterRoom(int id, bool is_roomMnr) {
	if (m_curPlayerNum >= MAX_PLAYER) return false;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_players[i]->GetEmpty()) {
			ATOMIC::g_clients_lock.lock();
			m_curPlayerNum++;
			m_players[i]->Enter();
			m_players[i]->SetID(id);
			m_players[i]->SetID_STR(SR::g_clients[id]->id_str);
			m_players[i]->SetMMR(SR::g_clients[id]->mmr);

			SR::g_clients[id]->room_id = m_roomNo;
			ATOMIC::g_clients_lock.unlock();

			if (is_roomMnr) m_RoomMnr = id;
			AnnounceRoomEnter(id);
			return true;
		}
	}
	return false;
}

void Room::AnnounceRoomEnter(int id) {
	//bool isMnr;
	int enterID = -1;

	for (int i = 1; i < MAX_PLAYER; ++i) {
		if (m_players[i]->GetID() == id) enterID = i;
	}

	for (int i = 1; i < MAX_PLAYER; ++i) {
		if (m_players[i]->GetEmpty() == false) {
			if (m_players[i]->GetID() != enterID) {
				int t_id{ m_players[i]->GetID() };
				bool t_ready{ m_players[i]->GetReady() };
				int t_mmr{ m_players[i]->GetMMR() };
				char t_id_str[MAX_ID_LEN];
				memcpy(t_id_str, m_players[i]->GetID_STR(), sizeof(char) * MAX_ID_LEN);
				bool is_enterID_mnr = false;

				//send for new user
				BattleServer::GetInstance()->SendRoomEnterPacket(enterID, t_id, t_ready, i, t_id_str, t_mmr, false);
				//send for old user
				BattleServer::GetInstance()->SendRoomEnterPacket(t_id, enterID, m_players[enterID]->GetReady(), enterID, m_players[enterID]->GetID_STR(), m_players[enterID]->GetMMR(), false);
			}
		}
	}
}

void Room::LeaveRoom(int id) {
	ATOMIC::g_clients_lock.lock();
	SR::g_clients[id]->room_id = -1;
	ATOMIC::g_clients_lock.unlock();

	int leaverIdx;
	int leaverID;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_players[i] == nullptr) return;
		leaverID = m_players[i]->GetID();
		if (leaverID == id) leaverIdx = i;
	}

	//new room mnr
	if (id == m_RoomMnr) {
		for (int i = 0; i < MAX_PLAYER; ++i) {
			leaverID = m_players[i]->GetID();
			if (leaverID != -1
				&& leaverID != id) {
				m_RoomMnr = m_players[i]->GetID();
				PushNewRoomMnrMsg(m_RoomMnr);
				m_isSent = false;
				break;
			}
		}
	}

	--m_curPlayerNum;
	m_players[leaverIdx]->Initialize();

	for (const auto& p : m_players) {
		leaverID = p->GetID();
		if (leaverID != -1 && leaverID != id) { //나간사람일 경우
			PushRoomLeaveMsg(leaverID, id);
		}
	}
}

std::wstring Room::GetRoomName() {
	return m_roomName;
}

PTC_VECTOR Room::GetCoinRandPos() {
	return PTC_VECTOR();
}

int Room::GetcurPlayerNum() {
	return m_curPlayerNum;
}

int Room::GetRoomNo() {
	return m_roomNo;
}

bool Room::IsGameStarted() {
	return m_isGameStarted;
}

bool Room::IsRoomStarted() {
	return m_isRoundStarted;
}

void Room::CreateAddCoinEvent() {
	if (this == nullptr) return;
	if (!IsGameStarted()) return;
	
	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(ADD_COIN_TIME), EVENT_TYPE::EV_ADD_COIN };
	BattleServer::GetInstance()->AddTimer(ev);
	
	PTC_VECTOR coin_pos{ static_cast<float>(rand() % 400 - 200) , 0, static_cast<float>(rand() % 400 - 200) };
	m_coins[m_coin_cur] = true;
	PushAddCoinMsg(coin_pos, m_coin_cur);
	m_coin_cur++;
}

void Room::AddCoinByDie(int die_id) {
	if (this == nullptr) return;
	if (!IsGameStarted()) return;

	cout << "DIE COIN" << endl;
	PTC_VECTOR coin_pos{ 
		static_cast<float>(m_players[die_id]->GetPosition().x + rand() % 400 - 200),
		0, 
		static_cast<float>(m_players[die_id]->GetPosition().z + rand() % 400 - 200) };
	m_coins[m_coin_cur] = true;
	PushAddCoinMsg(coin_pos, m_coin_cur);
	m_coin_cur++;
}

void Room::CreateReloadBulletEvent(){
	if (this == nullptr) return;
	if (!IsGameStarted()) return;
	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(RELOAD_BULLET_TIME), EVENT_TYPE::EV_RELOAD_BULLET };
	BattleServer::GetInstance()->AddTimer(ev);
	PushReloadBulletMsg();
}

void Room::SendLeftTimePacket() {
	if (this == nullptr) return;
	if (m_leftTime > 1 && m_isGameStarted) {
		EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(1), EVENT_TYPE::EV_TICK };
		BattleServer::GetInstance()->AddTimer(ev);
		PushSendTimeMsg(m_leftTime);
	}
}

void Room::CheckGameState() {
	if (this == nullptr) return;
	--m_leftTime;
	if (m_leftTime <= MAX_LEFT_TIME - COUNTDOWN_TIME) {
		if (!m_isRoundStarted) {
			m_isRoundStarted = true;
			RoundStart();
		}
	}

	if (m_isGameStarted) {
		if (m_leftTime <= 0) {
			GameOver(0); //draw
		}
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_players[i]->GetCoin() >= WIN_COIN_CNT && !m_players[i]->GetWinSatisfaction()) {
				m_players[i]->SetWinSatisfaction(true);
				PushNewWinSatisfaction(m_players[i]->GetID());
				m_players[i]->SetWinTime(m_leftTime - NEED_TIME_TO_WIN);
				//GameOver(m_players[i]->GetID());
			}
			if (m_players[i]->GetWinTime() >= m_leftTime && m_players[i]->GetWinSatisfaction()) {
				GameOver(m_players[i]->GetID());
			}
		}
	}
}

void Room::GameStart(){
	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_players[i]->SetCurObject(i);
		m_curPlayerNum++;
	}
	for (int i = 0; i < MAX_PLAYER; ++i) {
		PTC_START_INFO info;
		info.id = m_players[i]->GetID();
		info.spawn_pos = SR::g_spawn_pos.GetSpawnPosition(SpawnPosition::ePositionType(i));
		PushGameStartMsg(info.id, &info);
	}

	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(2), EVENT_TYPE::EV_WORLD_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev);

	m_isGameStarted = true;
	m_isEnterable = false;
}

void Room::RoundStart() {
	for (int i = 0; i < MAX_PLAYER; ++i) {
		const int& id = m_players[i]->GetID();
		if (id != -1) {
			PushRoundStartMsg();
		}
	}
	CreateAddCoinEvent();
}

void Room::GameOver(int winner) {
	cout << winner << " winner!\n";
	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(5), EVENT_TYPE::EV_RESET_ROOM };
	BattleServer::GetInstance()->AddTimer(ev);
	m_isGameStarted = false;
	m_isEnterable = true;
	PushGameOverMsg(winner);
}

void Room::UpdateUserInfo_DB(int winner) {
	for (auto& p : m_players) {
		const int& id = p->GetID();
		if (id != -1) {
			if (id == winner) {
				p->SetMMR(p->GetMMR() + WINNER_PLUS_MMR);
			}
			else {
				p->SetMMR(p->GetMMR() + LOOSER_MIN_MMR);
			}
		}
		ATOMIC::g_dbInfo_lock.lock();
		SR::g_clients[id]->mmr = p->GetMMR();
		ATOMIC::g_dbInfo_lock.unlock();

		EVENT ev{ id, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1), EVENT_TYPE::EV_UPDATE_DB };
		BattleServer::GetInstance()->AddTimer(ev);
		//BattleServer::GetInstance()->SendGameOverPacket(id, winner);
	}
}

void Room::PushSendMsg(int id, void* buff) {
	char* p = reinterpret_cast<char*>(buff);
	SEND_INFO sinfo{};
	sinfo.to = id;
	memcpy(&sinfo.buff, buff, (BYTE)p[0]);

#ifdef LOG_ON
	cout << "[ROOM] SEND: [msg: " << (int)p[1] << " id: " << id << "]\n";
#endif
	m_sendMsgQueueLock.lock();
	m_sendMsgQueue.push(sinfo);
	m_sendMsgQueueLock.unlock();
}

void Room::FlushSendMsg() {
	if (this == nullptr) return;

	m_sendMsgQueueLock.lock();
	std::queue<SEND_INFO> q{ m_sendMsgQueue };
	while (!m_sendMsgQueue.empty()) {
		m_sendMsgQueue.pop(); //여기서 문제가 생길 수 있음, 분리할 필요 있을수도?
	}
	m_sendMsgQueueLock.unlock();

	while (!q.empty()) {
		SEND_INFO sinfo = q.front();
		q.pop();

		BattleServer::GetInstance()->SendPacket(sinfo.to, &sinfo.buff);
	}
}

void Room::PushPlayerPositionMsg(int to, int from, PTC_VECTOR* position_info) {
#ifdef LOG_ON
	cout << "[ROOM] PushPlayerPositionMsg: " <<
		"to: " << to << " from: " << from <<
		" px: " << position_info->x <<
		" py: " << position_info->y <<
		" pz: " << position_info->x << endl;
#endif //LOG_ON
	bc_packet_player_pos p;
	p.size = sizeof(p);
	p.type = BC_PLAYER_POS;
	p.id = from;
	p.pos.x = position_info->x;
	p.pos.y = position_info->y;
	p.pos.z = position_info->z;
	PushSendMsg(to, &p);
}

void Room::PushPlayerDirectionMsg(int to, int from, PTC_VECTOR look){
	bc_packet_player_rot p;
	p.size = sizeof(p);
	p.type = BC_PLAYER_ROT;
	p.id = from;
	p.look = look;
	PushSendMsg(to, &p);
}

void Room::PushObjectPositionMsg(int id, short type_id, int obj_id, PTC_VECTOR* position_info) {
	bc_packet_object_pos p;
	p.size = sizeof(p);
	p.type = BC_OBJECT_POS;
	p.type_id = type_id;
	p.obj_id = obj_id;
	p.pos.x = position_info->x;
	p.pos.y = position_info->y;
	p.pos.z = position_info->z;
	PushSendMsg(id, &p);
}

void Room::PushShootBulletMsg(int from, short bullet_type, short bullet_idx, PTC_VECTOR look, PTC_VECTOR pos) {
	bc_packet_callback_bullet p;
	p.size = sizeof(p);
	p.type = BC_CALLBACK_BULLET;
	p.shootter_id = from;
	p.bullet_type = bullet_type;
	p.bullet_idx = bullet_idx;
	p.cam_look = look;
	p.bullet_pos = pos;

	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushRemoveBulletMsg(int bullet_id) {
	bc_packet_remove_bullet p;
	p.type = BC_REMOVE_BULLET;
	p.size = sizeof(p);
	p.bullet_id = bullet_id;

	int id{};
	for (const auto& pl : m_players) {
		id = pl->GetID();
		if (id != -1) PushSendMsg(id, &p);
	}
}

void Room::PushCallBackHitMsg(int be_hit_id) {
	bc_packet_callback_hit p;
	p.type = BC_CALLBACK_HIT;
	p.id = be_hit_id;

	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushDieMsg(int die_id) {
	bc_packet_callback_die p;
	p.type = BC_CALLBACK_DIE;
	p.size = sizeof(p);
	p.id = die_id;

	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushReadyMsg(int id, bool ready) {
	bc_packet_ready p;
	p.size = sizeof(p);
	p.type = BC_READY;
	p.id = id;
	p.ready = ready;

	int tid{};
	for (const auto& pl : m_players) {
		tid = pl->GetID();
		if (tid != -1) PushSendMsg(tid, &p);
	}
}

void Room::PushUnReadyMsg(int id) {
	bc_packet_ready p;
	p.size = sizeof(p);
	p.type = BC_READY;
	p.id = id;
	p.ready = false;

	int tid{};
	for (const auto& pl : m_players) {
		tid = pl->GetID();
		if (tid != -1) PushSendMsg(tid, &p);
	}
}

void Room::PushRoomStartAvailableMsg(int id, bool available) {
	bc_packet_room_start_available p;
	p.size = sizeof(p);
	p.type = BC_ROOM_START_AVAILABLE;
	p.available = available;
	PushSendMsg(id, &p);
}

void Room::PushNewRoomMnrMsg(int id) {
	bc_packet_new_room_host p;
	p.size = sizeof(p);
	p.type = BC_NEW_ROOM_HOST;
	p.id = id;

	int tid{};
	for (const auto& pl : m_players) {
		tid = pl->GetID();
		if (tid != -1) PushSendMsg(tid, &p);
	}
}

void Room::PushAnimPushMsg(int to, int from, int animType) {
	bc_packet_push_anim_type p;
	p.size = sizeof(p);
	p.type = BC_ANIM_PUSH;
	p.id = from;
	p.anim_type = animType;
#ifdef LOG_ON
	cout << "push ANIM: " << from << " -> " << to << " type: " << animType << endl;
#endif LOG_ON
	PushSendMsg(to, &p);
}

void Room::PushAnimPopMsg(int to, int from, int animType) {
	bc_packet_pop_anim_type p;
	p.size = sizeof(p);
	p.type = BC_ANIM_POP;
	p.id = from;
	p.anim_type = animType;
#ifdef LOG_ON
	cout << "pop ANIM: " << from << " -> " << to << " type: " << animType << endl;
#endif LOG_ON

	PushSendMsg(to, &p);
}

void Room::PushAddCoinMsg(PTC_VECTOR coin_pos, int coin_id) {
//#ifdef LOG_ON
	cout << "SendAddCoinPacket: coin_pos: [" << coin_pos.x << ", " << coin_pos.y << ", " << coin_pos.z << "]" << endl;
//#endif LOG_ON
	bc_packet_add_coin p;
	p.size = sizeof(p);
	p.type = BC_ADD_COIN;
	p.pos = coin_pos;
	p.coin_id = coin_id;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushUpdateCoinMsg(int update_id, int update_cnt, int delete_coin_id) {
	bc_packet_update_coin p;
	p.size = sizeof(p);
	p.type = BC_UPDATE_COIN;
	p.id = update_id;
	p.coin_cnt = update_cnt;
	p.delete_coin_id = delete_coin_id;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushNewWinSatisfaction(int satisfaction_id) {
	bc_packet_win_satisfaction p;
	p.size = sizeof(p);
	p.type = BC_WIN_SATISFACTION;
	p.satisfaction_id = satisfaction_id;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushGameOverMsg(int winner_id) {
	bc_packet_game_over p;
	p.size = sizeof(p);
	p.type = BC_GAME_OVER;
	p.win_team = winner_id;

	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushSendTimeMsg(char left_time) {
	bc_packet_left_time p;
	p.size = sizeof(p);
	p.type = BC_LEFT_TIME;
	p.left_time = left_time;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushReloadBulletMsg() {
	bc_packet_reload_bullet p;
	p.size = sizeof(p);
	p.type = BC_RELOAD_BULLET;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushPlayerLookMsg(int to, int from, PTC_VECTOR look) {
#ifdef LOG_ON
	cout << "SendPlayerRotation: " <<
		"to: " << to <<
		" from: " << from << endl;
#endif
	bc_packet_player_rot p;
	p.size = sizeof(p);
	p.type = BC_PLAYER_ROT;
	p.id = from;
	p.look = look;
	PushSendMsg(to, &p);
}

void Room::PushRoundStartMsg() {
	bc_packet_round_start p;
	p.size = sizeof(p);
	p.type = BC_ROUND_START;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::PushRoomLeaveMsg(int to, int leave) {
#ifdef LOG_ON
	cout << "SendRoomLeavePacket: " <<
		"to: " << to <<
		" leaver: " << leave << endl;
#endif
	bc_packet_room_leaved p;
	p.size = sizeof(p);
	p.type = BC_ROOM_LEAVED;
	p.id = leave;
	PushSendMsg(to, &p);
}

void Room::PushGameStartMsg(int id, PTC_START_INFO* player_info) {
	bc_packet_room_start p;
	p.size = sizeof(p);
	p.type = BC_ROOM_START;
	memcpy(&p.start_info, player_info, sizeof(PTC_START_INFO));
	PushSendMsg(id, &p);
}

void Room::PushRespawnMsg() {
	bc_packet_respawn p;
	p.size = sizeof(p);
	p.type = BC_RESPAWN;
	for (const auto& pl : m_players) {
		PushSendMsg(pl->GetID(), &p);
	}
}

void Room::MakeMove(int id) {
	if (this == nullptr) return;
	if (!m_isGameStarted) return;

	int tid{};
	for (const auto& pl : m_players) {
		tid = pl->GetID();
		if (tid != -1) pl->SetIsMoveable(true);
	}
}

void Room::MakeStop(int id) {
	if (this == nullptr) return;
	if (!m_isGameStarted) return;

	int tid{};
	for (const auto& pl : m_players) {
		tid = pl->GetID();
		if (tid != -1) pl->SetIsMoveable(false);
	}
}

void Room::CopyRecvMsgs() {
	//cout << "Copy\n";
	m_recvMsgQueueLock.lock();
	m_copiedRecvMsgs = m_recvMsgQueue;
	m_recvMsgQueueLock.unlock();

	m_recvMsgQueueLock.lock();
	while (!m_recvMsgQueue.empty()) {
		m_recvMsgQueue.pop();
	}
	m_recvMsgQueueLock.unlock();
}

void Room::ClearCopyMsg() {
	while (!m_copiedRecvMsgs.empty()) m_copiedRecvMsgs.pop();
}

void Room::ProcMsg(message msg) {
#ifdef LOG_ON
	cout << "[ROOM] Proc: [msg: " << (int)msg.type << " id: " << msg.id << "]\n";
#endif //LOG_ON
	switch (msg.type) {
	case CB_READY: {
		for (auto& pl : m_players) {
			if (pl->GetID() == msg.id) {
				if (pl->GetReady() == false) {
					pl->SetReady(true);
					PushReadyMsg(msg.id, pl->GetReady());
				}
			}
		}
		if (m_players[0]->GetReady() == true && m_players[1]->GetReady() == true) {
			ClearCopyMsg();
			if (!m_isGameStarted) {
				GameStart();
				SendLeftTimePacket();
			}
			break;
		}
		break;
	}
	case CB_POSITION_VECTOR: {
		int t_id{ msg.id };
		PTC_VECTOR t_v;
		int t_anim{ msg.anim_type };
		t_v.x = msg.vec.x;
		t_v.y = msg.vec.y;
		t_v.z = msg.vec.z;
		for (auto& pl : m_players) {
			if (t_id == pl->GetID()) {
				pl->SetPosition(XMFLOAT3{ t_v.x, t_v.y, t_v.z });
				//pl->SetAnimType(t_anim);
				//PushPlayerPositionMsg(t_id, pl->GetID(), &t_v);
			}
			else {
				PushPlayerPositionMsg(pl->GetID(), t_id, &t_v);
				//PushAnimMsg(pl->GetID(), t_id, pl->GetAnimType());
			}
		}
		break;
	}
	case CB_REQUEST_BULLET: {
		int t_id{ msg.id };
		int t_bullet_type{ msg.anim_type };
		PTC_VECTOR t_v{};
		t_v.x = msg.vec.x;
		t_v.y = msg.vec.y;
		t_v.z = msg.vec.z;

		XMFLOAT3 pos{};
		for (auto& pl : m_players) {
			if (pl->GetID() == t_id) {
				pos = pl->GetPosition();
				break;
			}
		}
		if (t_bullet_type == 1) {
			for (size_t i = 0; i < 2; ++i)
			{
				XMFLOAT3 T{ 0.f, 0.f, 0.f };
				T = SMathHelper::Add(T, msg.vec, i * 100);
				PTC_VECTOR t_pos{};
				t_pos.x = pos.x + T.x;
				t_pos.y = pos.y + 90.f + T.y;
				t_pos.z = pos.z + T.z;
				int bullet_idx = GetEmptyBullet();

				EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(MAKE_BULLET_EMPTY), EVENT_TYPE::EV_MAKE_EMPTY_BULLET };
				BattleServer::GetInstance()->AddTimer(ev);

				PushShootBulletMsg(t_id, t_bullet_type, bullet_idx, t_v, t_pos);
			}
		}
		if (t_bullet_type == 2) {
			cout << "BULLET TYPE : 2" << endl;
			PTC_VECTOR t_pos{};
			t_pos.x = pos.x ;
			t_pos.y = pos.y + 90.f;
			t_pos.z = pos.z;
			int bullet_idx = GetEmptyBullet();

			EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(MAKE_BULLET_EMPTY), EVENT_TYPE::EV_MAKE_EMPTY_BULLET };
			BattleServer::GetInstance()->AddTimer(ev);

			PushShootBulletMsg(t_id, t_bullet_type, bullet_idx, t_v, t_pos);
		}
		break;
	}
	case CB_PUSH_ANIM: {
		int t_id = msg.id;
		int t_anim = msg.anim_type;
		int a_id = 1;
		if (t_id == 1) a_id = 2;
		PushAnimPushMsg(a_id, t_id, t_anim);
		break;
	}
	case CB_POP_ANIM: {
		int t_id = msg.id;
		int t_anim = msg.anim_type;
		int a_id = 1;
		if (t_id == 1) a_id = 2;
		PushAnimPopMsg(a_id, t_id, t_anim);
		break;
	}
	case CB_REQUEST_DIE: {
		int t_id{ msg.id };

		for (auto& pl : m_players) {
			if (pl->GetID() == t_id) {
				for (int i = pl->GetCoin(); i > 0; --i) {
					AddCoinByDie(t_id);
				}
				pl->SetCoin(0);
				break;
			}
		}
		EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(RESPAWN_TIME), EVENT_TYPE::EV_RESPAWN };
		BattleServer::GetInstance()->AddTimer(ev);
		PushDieMsg(t_id);
		break;
	}
	case CB_GET_COIN: {
		if (!IsGameStarted()) break;
		int t_id{ msg.id };
		int t_delete_coin_id{ (int)msg.vec.x };
		cout << "delete coin id: " << t_delete_coin_id << endl;
		for (auto& pl : m_players) {
			if (pl->GetID() == t_id) {
				pl->SetCoin(pl->GetCoin() + 1);
				m_coins[t_delete_coin_id] = false;
				PushUpdateCoinMsg(t_id, pl->GetCoin(), t_delete_coin_id);
			}
			cout << " PLAYER: " << pl->GetID() << " COIN: " << pl->GetCoin();
		}
		cout << endl;
		break;
	}
	case CB_LOOK_VECTOR: {
		for (auto& pl : m_players) {
			int id = pl->GetID();
			if (id != -1) {
				if (id == msg.id) {
					if (pl->GetCurObject() == nullptr) return;
					XMFLOAT3 pre_look{ pl->GetCurObject()->GetLook() };
					pl->GetCurObject()->SetPreLook(pre_look);
					pl->GetCurObject()->SetMatrixByLook(msg.vec.x, msg.vec.y, msg.vec.z);
					//bb rotation
					//pl->GetCurObject()->m_boundaries->SetBBLook(pl->GetCurObject()->GetLook(), 0);
					//pl->GetCurObject()->m_boundaries->SetBBRight(pl->GetCurObject()->GetRight(), 0);
				}
				else {
					PTC_VECTOR recv_look{ msg.vec.x, msg.vec.y, msg.vec.z };
					PushPlayerLookMsg(id, msg.id, recv_look);
				}
			}
		}
		break;
	}
	default: {
		//unknown MSG
		while (true);
	}
	}
}

int Room::GetEmptyBullet()
{
	for (int i = 0; i < MAX_BULLET; ++i) {
		if (m_bullets[i] == false) {
			m_bullets[i] = true;
			m_q_bullet.push(i);
			return i;
		}
	}
	return 999;
}

void Room::SetEmptyBullet()
{
	int pop = m_q_bullet.front();
	m_q_bullet.pop();
	m_bullets[pop] = false;
}

void Room::Respawn() {
	cout << "Respawn" << endl;
	//리스폰 패킷 전달
	PushRespawnMsg();
}