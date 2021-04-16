#include "Room.h"

Room::Room(int room_no) {
	Initialize(room_no);
}

Room::Room(int room_no, wstring room_name) {
	Initialize(room_no);
	m_roomName = room_name;
}

Room::~Room() {
	if (m_physics != nullptr) {
		delete m_physics;
		m_physics = nullptr;
	}
	if (m_map != nullptr) {
		delete m_map;
		m_map = nullptr;
	}

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
	m_physics = new Physics();
	m_map = new Map();
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

	delete m_map;
	m_map = new Map();

	for (int i = 0; i < MAX_BULLET_COUNT; ++i) {
		//
	}

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

	//ProcUnUsedThunderbolt();
	CopyRecvMsgs();
	message procMsg;
	message procMsg;
	while (!m_copiedRecvMsgs.empty()) {
		procMsg = m_copiedRecvMsgs.front();
		m_copiedRecvMsgs.pop();
		ProcMsg(procMsg);
	}

	if (!m_isGameStarted) {
		int numOfReady;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (!m_players[i]->GetSlotStatus()) {
				if (m_players[i]->GetReady()) ++numOfReady;
			}
		}
		if (numOfReady == m_curPlayerNum
			&& m_curPlayerNum == 2) {
			if (!m_isSent) {
				PushGameStartAvailableMsg(m_RoomMnr, true);
				m_isSent = true;
			}
		}
		else {
			if (m_isSent) {
				m_isSent = false;
				PushGameStartAvailableMsg(m_RoomMnr, false);
			}
		}
	}
	else if (m_isGameStarted) {
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int id{ m_players[i]->GetID() };
			if (id != -1) {//not unset
				m_players[i]->Update(elapsedMilliSec);
				//anim
			}
		}
		WorldUpdate();
		Collision();

		XMFLOAT3 subDistance;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_players[i]->GetID() != -1) { //not unset
				if (!m_players[i]->IsDead()) {
					auto iter = m_players[i]->GetCurForm();

					XMFLOAT3 curpos = iter->GetPosition();
					XMFLOAT3 prepos = iter->GetPrePosition();

					//

					subDistance = Vector3::Subtract(curpos, prepos);

					if (fabs(subDistance.x) >= 0.1f
						|| fabs(subDistance.y) >= 3.f
						|| fabs(subDistance.z) >= 0.1f) {
						PTC_VECTOR ptc_pos;

						ptc_pos.x = curpos.x;
						ptc_pos.y = curpos.y;
						ptc_pos.z = curpos.z;

						iter->SetPrePosition(curpos);

						for (int j = 0; j < MAX_PLAYER; ++j) {
							if (m_players[j]->GetID() != -1) PushPlayerPositionMsg(m_players[j]->GetID(), m_players[i]->GetID(), &ptc_pos);
						}
					}
				}
			}
		}

		for (auto iter = m_map->m_ObjList[Map::NOTFIXED].begin(); iter->m_map->m_ObjList[Map::NOTFIXED].end(); ++iter) {
			XMFLOAT3 curpos = (*iter)->GetPosition();
			XMFLOAT3 prepos = (*iter)->GetPrePosition();

			subDistance = Vector3::Subtract(curpos, prepos);

			if (fabs(subDistance.x) >= 0.1f
				|| fabs(subDistance.y) >= 1.f
				|| fabs(subDistance.z) >= 0.1f) { //움직였다면
				short type_id = (*iter)->m_boundaries->GetObjType();
				int obj_id = (*iter)->GetUniqueID();
				PTC_VECTOR ptc_pos;

				ptc_pos.x = curpos.x;
				ptc_pos.y = curpos.y;
				ptc_pos.z = curpos.z;

				for (int i = 0; i < MAX_PLAYER; ++i) {
					if (m_players[i]->GetID() != -1) { //not unset
						PushObjectPositionMsg(m_players[i]->GetID(), type_id, obj_id, &ptc_pos); //클라 obj 움직임이 끝난다면
					}
				}
			}
		}

		for (int i = 0; i < m_bullets.size(); ++i) {
			if (m_bullets[i].isUsed()) {
				XMFLOAT3 pos = m_bullets[i].GetPosition();
				PTC_VECTOR ptc_pos;
				ptc_pos.x = pos.x;
				ptc_pos.y = pos.y;
				ptc_pos.z = pos.z;

				for (int j = 0; i < MAX_PLAYER; ++j) {
					if (m_players[j]->GetID() != -1) {
						PushObjectPositionMsg(m_players[j]->GetID(), m_bullets[i].m_boundaries->GetObjtype(), m_bullets[i].GetUniqueID(), &ptc_pos);
					}
				}
			}
		}
		++m_countFrame;
	}

	std::chrono::system_clock::time_point updateEnd{ std::chrono::system_clock::now() };
	std::chrono::duration<float> updateTime{ updateEnd - m_curUpdate };

	//60fps
	int nextUpdate{ static_cast<int>((0.016f - updateTime.count()) * 1000.0f) };
	if (nextUpdate <= 0) nextUpdate = 0;

	EVENT ev_update{ EVENT_KEY, m_roomNo, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(nextUpdate), EV_UPDATE };
	BattleServer::GetInstance()->AddTimer(ev_update);

	//
}

void Room::PushMsg(message& msg) {
	if (msg.id <= 1) return; //

	ATOMIC::g_msg_lock.lock();
	m_recvMsgQueue.push(msg);
	ATOMIC::g_msg_lock.unlock();
}

void Room::Collision() {
	//
}

void Room::WorldUpdate() {
	//
}

bool Room::IsEmpty() {
	if (m_curPlayerNum <= 0) return true;
	else return false;
}

//
bool Room::EnterRoom(int id, bool is_roomMnr) {
	if (m_curPlayerNum >= MAX_PLAYER) return false;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_players[i]->isEmpty()) {
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

//
void Room::AnnounceRoomEnter(int id) {
	bool isMnr;
	int enterID = -1;
	bool is_enterID_mnr = false;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_players[i]->GetID() == id) enterID = i;
	}
	if (m_RoomMnr == m_players[enterID]->GetID())  is_enterID_mnr = true;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (!m_players[i]->GetSlotStatus()) {
			Player& p = m_players[i];
			if (p->GetID() == m_RoomMnr) {
				BattleServer::GetInstance()->SendRoomEnterPacket(id, p->GetID(), p->GetReady(), i, p->GetID_STR(), p->GetMMR(), true);
			}
			else {
				BattleServer::GetInstance()->SendRoomEnterPacket(id, p->GetID(), p->GetReady(), i, p->GetID_STR(), p->GetMMR(), false);
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

//
void Room::LoadMap() {
	//m_map = new Map();
	//m_map->Initialize();
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
	if (m_leftTime > 1
		&& m_isGameStarted) {
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int id = m_players[i]->GetID();
			if (id != -1) BattleServer::GetInstance()->SendLeftTimePacket(id, m_leftTime);
		}
	}
}

void Room::CheckGameState() {

}

void Room::GameStart()
{
}

void Room::RoundStart()
{
}

void Room::GameOver(int winner_role)
{
}

void Room::UpdateUserInfo_DB(int winner_role)
{
}

void Room::PushSendMsg(int id, void* buff)
{
}

void Room::FlushSendMsg()
{
}

void Room::PushPlayerPositionMsg(int to, int from, PTC_VECTOR* position_info)
{
}

void Room::PushPlayerDirectionMsg(int to, int from, PTC_VECTOR look)
{
}

void Room::PushObjectPositionMsg(int id, short type_id, int obj_id, PTC_VECTOR* position_info)
{
}

void Room::PushTransformMsg(int transformer, short obj_type)
{
}

void Room::PushShootBulletMsg(int to, PTC_VECTOR look)
{
}

void Room::PushHitMsg(int hit_id, int dmg)
{
}

void Room::PushDieMsg(int die_id)
{
}

void Room::PushReadyMsg(int id)
{
}

void Room::PushUnReadyMsg(int id)
{
}

void Room::PushGameStartAvailableMsg(int id, bool available)
{
}

void Room::PushNewRoomMnrMsg(int id)
{
}

void Room::PushAnimMsg(int id, int animType)
{
}

void Room::MakeMove(int id)
{
}

void Room::MakeStop(int id)
{
}

void Room::CopyRecvMsgs()
{
}

void Room::ClearCopyMsg()
{
}

void Room::ProcMsg(message msg)
{
}
