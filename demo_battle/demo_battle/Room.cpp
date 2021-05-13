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
	m_ToyNum = 0;
	m_isGameStarted = false;
	m_isRoundStarted = false;
	m_leftTime = MAX_LEFT_TIME;
	//m_physics = new Physics();
	//m_map = new Map();
	m_lastUpdate = std::chrono::system_clock::now();

	for (auto& p : m_players) p = new Toy();

	int bulletUniqueID{ OBJECT_START_INDEX_BULLET };
	for (int i = 0; i < MAX_BULLET_COUNT; ++i) m_bullets[i].SetuniqueID(bulletUniqueID++);

	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30ms), EV_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev);
}

void Room::Reset() {
	if (this == nullptr) return;

	m_isEnterable = true;
	m_isRoundStarted = false;
	m_ToyNum = 0;
	m_leftTime = MAX_LEFT_TIME;
	m_lastUpdate = std::chrono::system_clock::now();

	//delete m_map;
	//m_map = new Map();

	for (auto& p : m_players) {
		Toy* tmp = new Toy(*p);
		delete p;
		p = tmp; // player -> Toy type cast
	}
}

void Room::Update() {
#ifdef LOG_ON
	std::cout << "Room::Update() called\n";
#endif
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
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int id{ m_players[i]->GetID() };
			if (id != -1) {//not unset
				m_players[i]->Update(elapsedMilliSec);
				//anim
			}
		}
		//WorldUpdate();
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
		++m_countFrame;
	}

	std::chrono::system_clock::time_point updateEnd{ std::chrono::system_clock::now() };
	std::chrono::duration<float> updateTime{ updateEnd - m_curUpdate };

	//60fps
	int nextUpdate{ static_cast<int>((0.016f - updateTime.count()) * 1000.0f) };
	if (nextUpdate <= 0) nextUpdate = 0;

	EVENT ev_update{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(nextUpdate), EV_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev_update);

	EVENT ev_flush{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1), EV_FLUSH_MSG };
	BattleServer::GetInstance()->AddTimer(ev_flush);
}

void Room::PushMsg(message& msg) {
	if (msg.id <= 0) return; // 0은 lobby

	ATOMIC::g_msg_lock.lock();
	m_recvMsgQueue.push(msg);
	ATOMIC::g_msg_lock.unlock();
}

void Room::Collision() {
	//
}

void Room::WorldUpdate() {
	//유동 object update
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
			m_curPlayerNum++;
			m_players[i]->Enter();
			m_players[i]->SetID(id);
			m_players[i]->SetID_STR(SR::g_clients[id]->id_str);
			m_players[i]->SetMMR(SR::g_clients[id]->mmr);
			
			ATOMIC::g_clients_lock.lock();
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

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_players[i]->GetID() == id) enterID = i;
	}

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_players[i]->GetEmpty() == false) {
			if (m_players[i]->GetID() != enterID) {
				int t_id{ m_players[i]->GetID() };
				bool t_ready{ m_players[i]->GetReady() };
				int t_mmr{ m_players[i]->GetMMR() };
				char t_id_str[MAX_ID_LEN];
				bool is_enterID_mnr = false;

				memcpy(t_id_str, m_players[i]->GetID_STR(), sizeof(char) * MAX_ID_LEN);
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

	//isGameStarted is true
	--m_curPlayerNum;
	if (m_isGameStarted) {
		//?
	}

	m_players[leaverIdx]->Initialize();

	for (const auto& p : m_players) {
		leaverID = p->GetID();
		if (leaverID != -1
			&& leaverID != id) { //나간사람일 경우
			BattleServer::GetInstance()->SendRoomLeavePacket(leaverID, id);
		}
	}
}

std::wstring Room::GetRoomName() {
	return m_roomName;
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

void Room::SendLeftTimePacket() {
	if (this == nullptr) return;
	if (m_leftTime > 1 && m_isGameStarted) {
		EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(1), EV_TICK };
		BattleServer::GetInstance()->AddTimer(ev);
		m_countFrame = 0;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int id = m_players[i]->GetID();
			if (id != -1) BattleServer::GetInstance()->SendLeftTimePacket(id, m_leftTime);
		}
	}
}

void Room::CheckGameState() {
	if (this == nullptr) return;
	--m_leftTime;
	if (m_leftTime <= MAX_LEFT_TIME - COUNTDOWN_TIME) {
		if (!m_isRoundStarted) {
			m_isRoundStarted = true;
			cout << "---RoundStart---\n";
			RoundStart();
		}
	}

	if (m_isGameStarted) {
		if (m_leftTime <= 0) {
			GameOver(0);
		}
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_players[i]->GetCoin() > WIN_COIN_CNT) {
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
		BattleServer::GetInstance()->SendGameStartPacket(info.id, &info);
	}

	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(2), EV_WORLD_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev);

	m_isGameStarted = true;
	m_isEnterable = false;
}

//unused
void Room::RoundStart() {
	for (int i = 0; i < MAX_PLAYER; ++i) {
		const int& id = m_players[i]->GetID();
		if (id != -1) {
			BattleServer::GetInstance()->SendRoundStartPacket(id);
		}
	}
}

void Room::GameOver(int winner) {
	cout << winner << " winner!\n";
	EVENT ev{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::seconds(5), EV_RESET_ROOM };
	BattleServer::GetInstance()->AddTimer(ev);
	for (const auto& pl : m_players) {
		BattleServer::GetInstance()->SendGameOverPacket(pl->GetID(), winner);
	}
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

		EVENT ev{ id, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1), EV_UPDATE_DB };
		BattleServer::GetInstance()->AddTimer(ev);
		BattleServer::GetInstance()->SendGameOverPacket(id, winner);
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
	cout << "[ROOM] PushPlayerPositionMsg: " <<
		"to: " << to << " from: " << from <<
		" px: " << position_info->x <<
		" py: " << position_info->y <<
		" pz: " << position_info->x << endl;
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

void Room::PushShootBulletMsg(int to, int bullet_id, PTC_VECTOR look) {
	bc_packet_shoot_bullet p;
	p.size = sizeof(p);
	p.type = BC_SHOOT_BULLET;
	p.bullet_id = bullet_id;
	p.pos = look;
	PushSendMsg(to, &p);
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

void Room::PushHitMsg(int hit_id, int dmg) {
	bc_packet_hit p;
	p.type = BC_HIT;
	p.hp = dmg;
	p.id = hit_id;

	int id{};
	for (const auto& pl : m_players) {
		id = pl->GetID();
		if (id != -1) PushSendMsg(id, &p);
	}
}

void Room::PushDieMsg(int die_id) {
	bc_packet_die p;
	p.type = BC_DIE;
	p.size = sizeof(p);
	p.id = die_id;

	int id{};
	for (const auto& pl : m_players) {
		id = pl->GetID();
		if (id != -1) {
			PushSendMsg(id, &p);
		}
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

void Room::PushAnimMsg(int id, int animType) {
	bc_packet_anim_type p;
	p.size = sizeof(p);
	p.type = BC_ANIM;
	p.id = id;
	p.anim_type = animType;

	int tid{};
	for (const auto& pl : m_players) {
		tid = pl->GetID();
		if (tid != -1) PushSendMsg(tid, &p);
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
	case CB_START: {
		ClearCopyMsg();
		if (!m_isGameStarted) {
			GameStart();
			SendLeftTimePacket();
		}
		break;
	}
	case CB_POSITION_VECTOR: {
		for (auto& pl : m_players) {
			int t_id = msg.id;
			PTC_VECTOR t_v;
			t_v.x = msg.vec.x;
			t_v.y = msg.vec.y;
			t_v.z = msg.vec.z;
			cout << "!!: " << t_v.x << " " << t_v.y << " " << t_v.z << endl;
			if (t_id == pl->GetID()) {
				pl->SetPosition(XMFLOAT3{ t_v.x, t_v.y, t_v.z });
				PushPlayerPositionMsg(t_id, pl->GetID(), &t_v);
			}
			else {
				PushPlayerPositionMsg(pl->GetID(), t_id, &t_v);
			}
		}
		break;
	}
	case CB_BULLET: {
		if (!m_isRoundStarted) break;
		int bullet_id{};
		for (auto& pl : m_players) {
			for (int i = 0; i < MAX_BULLET_COUNT; ++i) {
				if (!m_bullets[i].isBulletActive()) {
					bullet_id = i;
					XMFLOAT3 look{ pl->GetCurObject()->GetLook() };
					m_bullets[i].Shoot(pl->GetCurObject()->GetPosition(), look, msg.vec, 1.f);

					static_cast<Toy*>(pl)->SetIsShoot(true);
					break;
				}
				else if (i + 1 == MAX_BULLET_COUNT) {
					cout << "bullet max\n";
				}
			}
		}

		XMFLOAT3 bullet_pos{ m_bullets[bullet_id].GetPosition() };
		PTC_VECTOR ptc_bullet_pos{};
		ptc_bullet_pos.x = bullet_pos.x;
		ptc_bullet_pos.y = bullet_pos.y;
		ptc_bullet_pos.z = bullet_pos.z;
		
		int id{};
		for (const auto& pl : m_players) {
			id = pl->GetID();
			if (id != -1) {
				PushShootBulletMsg(id, bullet_id, ptc_bullet_pos);
			}
		}
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
					BattleServer::GetInstance()->SendPlayerLook(id, msg.id, recv_look);
				}
			}
		}
		break;
	}
	case CB_TEST_TIME_MINUS: {
		break;
	}
	case CB_TEST_TIME_PLUS: {
		break;
	}
	default: {
		//unknown MSG
		while (true);
	}
	}
}
