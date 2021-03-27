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
		WSAOVERLAPPED* p_over;

		bool bSuccess = GetQueuedCompletionStatus(SHARED_RESOURCE::iocp, &num_byte, &key, &p_over, INFINITE);
		if (!bSuccess) {
			int err = WSAGetLastError();
			BattleServer::GetInstance()->error_display("GQCS ERROR", err);
		}

		SOCKET clientSocket;
		if (key == EVENT_KEY);
		else if (key == MATCH_MAKING_SERVER_KEY) {
#ifdef LOG_ON
			std::cout << "MatchMakingServerKey Return\n";
#endif
			clientSocket = SHARED_RESOURCE::clients[key]->m_s->GetSocket();
		}
		else {
			if (SHARED_RESOURCE::clients[key]->m_s = nullptr) {
#ifdef LOG_ON
				std::cout << key << "- socket was nullptr\n";
#endif
				continue;
			}
			clientSocket = SHARED_RESOURCE::clients[key]->m_s->GetSocket();
			if (num_byte == 0) {
				//disconnect
				DisconnectClient(key, clientSocket);
				continue;
			}
		}
		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(p_over);

		switch (ex_over->ev_type) {

		}
	}
}