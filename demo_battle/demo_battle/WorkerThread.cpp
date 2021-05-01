#include "pch.h"
#include "WorkerThread.h"

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
			int err = WSAGetLastError();
			BattleServer::GetInstance()->error_display("GQCS ERROR", err);
		}

		SOCKET clientSocket;
		int evkey = static_cast<int>(key);
		if (evkey == EVENT_KEY);
		else if (evkey == LOBBY_SERVER_KEY) {
#ifdef LOG_ON
			std::cout << "MatchMakingServerKey Return\n";
#endif
			clientSocket = SR::g_clients[key]->m_s->GetSocket();
		}
		else {
			if (SR::g_clients[key]->m_s = nullptr) {
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
		case EV_TCP_RECV: {
			break;
		}
		case EV_SEND: {
			break;
		}
		case EV_UPDATE: {
			break;
		}
		case EV_TICK: {
			break;
		}
		case EV_FLUSH_MSG: {
			break;
		}
		case EV_MOVE_ENABLE: {
			break;
		}
		case EV_MAKE_MOVE_DISABLE: {
			break;
		}
		case EV_UPDATE_DB: {
			break;
		}
		case EV_RESET_ROOM: {
			break;
		}
		default: {
			std::cout << "worker > unknown event type" << ex_over->ev_type << std::endl;
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

	switch (inputPacket[1]) {
	case LB_REQUEST_ROOM: {
#ifdef LOG_ON
		std::cout << "recv lb_packet_request_room from Lobby\n";
#endif
		int roomNo;
		for (int i = 0; i < MAX_ROOM; ++i) { //선형 순회로 빈 방 찾기
			ATOMIC::g_room_no_lock.lock();
			if (SR::g_room_no[i] == -1) {
				SR::g_room_no[i] = i + 1;
				roomNo = SR::g_room_no[i];
				ATOMIC::g_room_no_lock.unlock();
				break;
			}
			ATOMIC::g_room_no_lock.unlock();
		}

		Room* new_room = new Room(roomNo);
		SR::g_rooms[roomNo] = new_room;
		++ATOMIC::g_RoomNum;
		BattleServer::GetInstance()->SendAutoRoomReadyPacket(id, roomNo);
		break;
	}
	case CB_LOGIN: {
		cb_packet_login* p = reinterpret_cast<cb_packet_login*>(inputPacket);
#ifdef LOG_ON
		std::cout << "recv cb_packet_login from ID:" << id << std::endl;
#endif
		ATOMIC::g_dbInfo_lock.lock();
		memcpy(&SR::g_clients[id]->id_str, &p->name, sizeof(char) * MAX_ID_LEN);
		SR::g_clients[id]->mmr = p->mmr;
		SR::g_clients[id]->winningGameCnt = p->winningGameCnt;
		SR::g_clients[id]->totalGameCnt = p->totalGameCnt;
		ATOMIC::g_dbInfo_lock.unlock();

		BattleServer::GetInstance()->SendAutoAccessOKPacket(id);
		break;
	}
	case CB_JOIN: {
#ifdef LOG_ON
		std::cout << "recv cb_packet_join from ID: "id << std::endl;
#endif

		int roomNo;
		memcpy(&roomNo, &inputPacket[2]/*inputPacket + 2*/, sizeof(int));

		//if (!SR::g_rooms[roomNo]) break;

		if (SR::g_rooms.find(roomNo) == SR::g_rooms.end()) {
#ifdef LOG_ON
			std::cout << "room not found" << std::endl;
#endif
			BattleServer::GetInstance()->SendRoomJoinFail(id, ROOM_FAIL_CODE_ROOM_NOT_FOUND);
			break;
		}

		if (SR::g_rooms[roomNo]->IsRoomStarted()) {
#ifdef LOG_ON
			std::cout << "room already started" << std::endl;
#endif
			BattleServer::GetInstance()->SendRoomJoinFail(id, ROOM_FAIL_CODE_ROOM_STARTED);
			break;
		}

		bool isJoin{ false };
		//int slot;
		//cb_packet_join
		isJoin = SR::g_rooms[roomNo]->EnterRoom(id, inputPacket[6]/*isRoomMnr*/);
		if (isJoin) {
			BattleServer::GetInstance()->SendRoomJoinSuccess(id, inputPacket[6]); //Room Mnr이면 1P
		}
		else {
			BattleServer::GetInstance()->SendRoomJoinFail(id, ROOM_FAIL_CODE_ROOM_IS_FULL);
		}

		break;
	}
	case CB_ROOM_LEAVE: {
#ifdef LOG_ON
		std::cout << "recv cb_packet_room_leave from ID: " << id << std::endl;
#endif
		int roomNo = SR::g_clients[id]->room_id;
		SR::g_rooms[roomNo]->LeaveRoom(id);

		if (SR::g_rooms[roomNo]->IsEmpty()) {
			EraseRoom(roomNo);
		}
		break;
	}
	case CB_KEY_W_UP: 
	case CB_KEY_A_UP: 
	case CB_KEY_S_UP: 
	case CB_KEY_D_UP: 
	case CB_KEY_W_DOWN: 
	case CB_KEY_S_DOWN: 
	case CB_KEY_A_DOWN: 
	case CB_KEY_D_DOWN: {
		msg.type = inputPacket[1];
		break;
	}
	case CB_KEY_JUMP: {
		msg.type = CB_KEY_JUMP;
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
#ifdef LOG_ON
		std::cout << id << "start_packet received\n";
#endif
		msg.type = CB_START;
		break;
	}
	case CB_READY: {
#ifdef LOG_ON
		std::cout << id << "start_packet received\n";
#endif
		msg.type = CB_READY;
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
	std::cout << "Disconnect - Client ID: " << disconnectClientID << "room NO: "roomID << std::endl;
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