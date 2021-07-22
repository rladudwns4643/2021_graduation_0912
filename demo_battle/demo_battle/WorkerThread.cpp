#include "pch.h"
#include "WorkerThread.h"

//#define LOG_ON

void WorkerThread::InitThread() {
	m_ServerAddr = BattleServer::GetInstance()->GetServerAdder();
	m_AddrLen = sizeof(m_ServerAddr);
	mythread = std::thread([&]() {WorkerThread::ProcThread(); });
}

void WorkerThread::ProcThread() {
	while (true) {
		DWORD num_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		bool bSuccess = GetQueuedCompletionStatus(SR::g_iocp, &num_byte, (PULONG_PTR)p_key, &p_over, INFINITE);
		if (!bSuccess) {
			int err = GetLastError();
			BattleServer::GetInstance()->error_display("GQCS ERROR", err);
		}

		SOCKET clientSocket;
		int evkey = static_cast<int>(key);
		if (evkey == EVENT_KEY);
		else if (evkey == LOBBY_SERVER_KEY) {
			clientSocket = SR::g_clients[key]->m_s->GetSocket();
		}
		else {
			if (SR::g_clients[key]->m_s == nullptr) {
#ifdef LOG_ON
				std::cout << key << "- socket was nullptr\n";
#endif
				continue;
			}
			clientSocket = SR::g_clients[key]->m_s->GetSocket();
			if (num_byte == 0) {
				//disconnect
				DisconnectClient(key, clientSocket);
				continue;
			}
		}
		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(p_over);

		switch (ex_over->ev_type) {
		case EVENT_TYPE::EV_RECV: {
			char* buf = SR::g_clients[key]->m_recv_over.net_buf;
			unsigned int psize = SR::g_clients[key]->curr_packet_size;
			unsigned int pr_size = SR::g_clients[key]->prev_packet_data;
			message msg{ -1 };
			while (num_byte > 0) {
				if (psize == 0) psize = (BYTE)buf[0];
				if (num_byte + pr_size >= psize) { //패킷완성
					unsigned char p[MAX_BUFFER];
					memcpy(p, SR::g_clients[key]->packet_buf, pr_size);
					memcpy(p + pr_size, buf, psize - pr_size);
					msg = ProcPacket(key, p);
					if (msg.type != NO_MSG) {
						int room_id = SR::g_clients[key]->room_id;
						//cout << "Push MSG [room:" << room_id << " msg:" << (int)msg.type << " id: " << msg.id << "]\n";
						SR::g_rooms[room_id]->PushMsg(msg);
					}

					num_byte -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0;
					pr_size = 0;
				}
				else {
					ATOMIC::g_clients_lock.lock();
					memcpy(SR::g_clients[key]->packet_buf + pr_size, buf, num_byte);
					ATOMIC::g_clients_lock.unlock();
					pr_size += num_byte;
					num_byte = 0;
				}
			}
			SR::g_clients[key]->curr_packet_size = psize;
			SR::g_clients[key]->prev_packet_data = pr_size;

			DWORD flags = 0;
			ZeroMemory(&ex_over->over, sizeof(WSAOVERLAPPED));
			ZeroMemory(&msg, sizeof(message));
			ex_over->ev_type = EVENT_TYPE::EV_RECV;

			int ret = WSARecv(clientSocket, ex_over->wsabuf, 1, 0, &flags, &ex_over->over, 0);
			if (ret == SOCKET_ERROR) {
				int err_no = WSAGetLastError();
				if (err_no != ERROR_IO_PENDING) {
					BattleServer::GetInstance()->error_display("WSARecv Error", err_no);
					while (true);
				}
			}
			break;
		}
		case EVENT_TYPE::EV_SEND: {
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_UPDATE: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->Update();
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_TICK: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->SendLeftTimePacket();
			SR::g_rooms[room_id]->CheckGameState();
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_FLUSH_MSG: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->FlushSendMsg();
			break;
		}
		case EVENT_TYPE::EV_ADD_COIN: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->SendAddCoinPacket();//
			break;
		}
		case EVENT_TYPE::EV_MOVE_ENABLE: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->MakeMove(key);
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_MAKE_MOVE_DISABLE: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->MakeStop(key);
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_UPDATE_DB: {
			BattleServer::GetInstance()->SendUpdateUserInfoPacket(key, SR::g_clients[key]->mmr);
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_RESET_ROOM: {
			int room_id = *(int*)ex_over->net_buf;
			//SR::g_rooms[room_id]->WorldUpdate();
			delete ex_over;
			break;
		}
		case EVENT_TYPE::EV_WORLD_UPDATE: {
			int room_id = *(int*)ex_over->net_buf;
			SR::g_rooms[room_id]->WorldUpdate();
			break;
		}
		default: {
			std::cout << "[worker] unknown event type" << (int)ex_over->ev_type << std::endl;
		}
		}
	}
}

void WorkerThread::JoinThread() {
	mythread.join();
}

message WorkerThread::ProcPacket(int id, void* buf) {
	char* inputPacket = reinterpret_cast<char*>(buf);
	message msg;
	memset(&msg, 0, sizeof(message));
	msg.id = id;
	msg.type = NO_MSG;
	msg.anim_type = -1;

#ifdef LOG_ON
	std::cout << "[WORKER] ProcPacket Type: " << (int)inputPacket[1] << " ID: " << id << endl;
#endif //LOG_ON
	switch (inputPacket[1]) {
	case LB_REQUEST_ROOM: {
		int roomNo;
		for (int i = 0; i < MAX_ROOM; ++i) { //선형 순회로 빈 방 찾기, 이거 최악의 경우에 안들어갈 수 있음 탐색 개편 필요
			ATOMIC::g_room_no_lock.lock();
			if (SR::g_room_no[i] == -1) {
				SR::g_room_no[i] = i + 1;
				roomNo = SR::g_room_no[i];
				ATOMIC::g_room_no_lock.unlock();
				break;
			}
			ATOMIC::g_room_no_lock.unlock();
		}
#ifdef LOG_ON
		cout << "make room: " << roomNo << endl;
#endif 
		Room* new_room = new Room(roomNo);
		SR::g_rooms[roomNo] = new_room;
		++ATOMIC::g_RoomNum;
		BattleServer::GetInstance()->SendAutoRoomReadyPacket(id, roomNo);
		break;
	}
	case CB_LOGIN: {
		cb_packet_login* p = reinterpret_cast<cb_packet_login*>(inputPacket);
		ATOMIC::g_dbInfo_lock.lock();
		memcpy(&SR::g_clients[id]->id_str, &p->name, sizeof(char) * MAX_ID_LEN);
		SR::g_clients[id]->mmr = p->mmr;
		ATOMIC::g_dbInfo_lock.unlock();

		BattleServer::GetInstance()->SendBattleLoginOKPacket(id);
		break;
	}
	case CB_JOIN: {
		int roomNo;
		memcpy(&roomNo, &inputPacket[2]/*inputPacket + 2*/, sizeof(int));

		//if (!SR::g_rooms[roomNo]) break;

		if (SR::g_rooms.find(roomNo) == SR::g_rooms.end()) {
			BattleServer::GetInstance()->SendRoomJoinFail(id, ROOM_FAIL_CODE_ROOM_NOT_FOUND);
			break;
		}

		if (SR::g_rooms[roomNo]->IsRoomStarted()) {
			BattleServer::GetInstance()->SendRoomJoinFail(id, ROOM_FAIL_CODE_ROOM_STARTED);
			break;
		}

		bool isJoin{ false };
		//int slot;
		//cb_packet_join
		isJoin = SR::g_rooms[roomNo]->EnterRoom(id, inputPacket[3]/*isRoomMnr*/);
		if (isJoin) {
			BattleServer::GetInstance()->SendRoomJoinSuccess(id, inputPacket[3]); //Room Mnr이면 1P
		}
		else {
			BattleServer::GetInstance()->SendRoomJoinFail(id, ROOM_FAIL_CODE_ROOM_IS_FULL);
		}

		break;
	}
	case CB_ROOM_LEAVE: {
		int roomNo = SR::g_clients[id]->room_id;
		SR::g_rooms[roomNo]->LeaveRoom(id);

		if (SR::g_rooms[roomNo]->IsEmpty()) {
			EraseRoom(roomNo);
		}
		break;
	}
	case CB_MAKE_ANIM: {
		cb_packet_anim* p = reinterpret_cast<cb_packet_anim*>(inputPacket);
		if (p == nullptr) {
			msg.type = NO_MSG;
		}
		msg.id = p->id;
		msg.type = CB_MAKE_ANIM;
		msg.anim_type = p->anim_type;
		break;
	}
	case CB_POSITION_VECTOR: {
		cb_packet_position* p = reinterpret_cast<cb_packet_position*>(inputPacket);
		if (p == nullptr) {
			msg.type = NO_MSG;
			break;
		}
		msg.id = p->id;
		msg.type = CB_POSITION_VECTOR;
		msg.vec.x = p->pos.x;
		msg.vec.y = p->pos.y;
		msg.vec.z = p->pos.z;
		msg.anim_type = p->anim_type;
		break;
	}
	case CB_LOOK_VECTOR: {
		cb_packet_look_vector* p = reinterpret_cast<cb_packet_look_vector*>(inputPacket);

		if (p == nullptr) {
			msg.type = NO_MSG;
			break;
		}
		msg.id = p->id;
		msg.type = CB_LOOK_VECTOR;
		msg.vec.x = p->look.x;
		msg.vec.y = p->look.y;
		msg.vec.z = p->look.z;
		break;
	}
	case CB_START: {
		msg.type = CB_START;
		break;
	}
	case CB_READY: {
		msg.type = CB_READY;
		break;
	}
	case CB_GET_COIN: {
		cb_packet_get_coin* p = reinterpret_cast<cb_packet_get_coin*>(inputPacket);
		msg.id = p->id;
		msg.type = CB_GET_COIN;
		msg.vec.x = p->coin_id;
		break;
	}
	case CB_TEST_TIME_PLUS:
	case CB_TEST_TIME_MINUS: {
		msg.type = inputPacket[1];
		break;
	}
	default: {
#ifdef LOG_ON
		std::cout << "Invalid Packet Type Error!\n";
#endif
		while (true);
	}
	}
	return msg;
}

void WorkerThread::DisconnectClient(int disconnectClientID, SOCKET clientSocket) {
	int roomID = SR::g_clients[disconnectClientID]->room_id;

	if (roomID != -1) {
		SR::g_rooms[roomID]->LeaveRoom(disconnectClientID);
		if (SR::g_rooms[roomID]->IsEmpty()) {
			EraseRoom(roomID);
		}
	}

	ATOMIC::g_clients_lock.lock();
	SR::g_clients[disconnectClientID]->isConnected = false;
	ZeroMemory(SR::g_clients[disconnectClientID], sizeof(CLIENT));

	SR::g_clients[disconnectClientID]->m_s = nullptr;
	ATOMIC::g_clients_lock.unlock();
#ifdef LOG_ON
	std::cout << "Disconnect - Client ID: " << disconnectClientID << "room NO: "<<roomID << std::endl;
#endif 
}

void WorkerThread::EraseRoom(int& roomID) {
	ATOMIC::g_room_lock.lock();
	SR::g_rooms.erase(roomID);
	ATOMIC::g_room_lock.unlock();

	for (int i = 0; i < MAX_ROOM; ++i) {
		ATOMIC::g_room_no_lock.lock();
		if (SR::g_room_no[i] == roomID) SR::g_room_no[i] = -1;
		ATOMIC::g_room_no_lock.unlock();
	}
	--ATOMIC::g_RoomNum;
#ifdef LOG_ON
	std::cout << "room NO - " << roomID << "erased!\n";
#endif
}