#include "LobbyServer.h"
#include "protocol.h"

LobbyServer::LobbyServer(short lobby_id)
{
	lobbyID = lobby_id;

	WSAStartup(MAKEWORD(2, 2), &WSAData);
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(LOBBY_SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(SOCKADDR_IN));
	listen(listenSocket, SOMAXCONN);

	memset(&clientAddr, 0, addrlen);
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

LobbyServer::~LobbyServer() {
	closesocket(listenSocket);
	WSACleanup();
}

void LobbyServer::ClinetAccept(int id) {
	clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrlen);
	CLIENT* new_user = new CLIENT;
	new_user->id = id;
	new_user->m_s = clientSocket;
	new_user->m_recv_over.wsabuf[0].len = MAX_BUF_SIZE;
	new_user->m_recv_over.wsabuf[0].buf = new_user->m_recv_over.io_buf;
	new_user->m_recv_over.is_recv = true;
	new_user->user_info = new User();
	new_user->isActive = true;
	new_user->m_curr_packet_size = 0;
	new_user->m_prev_packet_data = 0;
	
	userList[id] = new_user;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, id, 0);
	memset(&userList[id]->m_recv_over.over, 0, sizeof(WSAOVERLAPPED));
	flags = 0;
	int ret = WSARecv(clientSocket, userList[id]->m_recv_over.wsabuf, 1, NULL, &flags, &(userList[id]->m_recv_over.over), NULL);
	if (ret != 0) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("WSARecv Error: ", err_no);
	}

	cout << "user: " << id <<"connect"<< endl;
	//this->ClinetAccept(id + 1);
}

void LobbyServer::DoWorker()
{
	while (true) {
		DWORD io_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		BOOL ret = GetQueuedCompletionStatus(iocp, &io_byte, reinterpret_cast<PULONG_PTR>(p_key), &p_over, INFINITE);
		SOCKET clientSocket = userList[key]->m_s;

		if (ret == false) {
			int err_no = WSAGetLastError();
			if (err_no == 64) closesocket(clientSocket);
			else error_display("GQCS: ", err_no);
		}

		//Ŭ���̾�Ʈ���� closesocket ���� ���
		if (io_byte == 0) {
			closesocket(clientSocket);
			userList[key]->isActive = false;
			userList[key]->user_info->SetPlayerLoginOK("");
			if (userList[key]->user_info->GetPlayerMatch() == true) {
				//SendClientDisconnectPacket(key);
			}
			delete userList[key]->user_info;
			continue;
		}

		EXOVER* exover = reinterpret_cast<EXOVER*>(p_over);

		//recv
		if (exover->is_recv == true) {
			char* buf = userList[key]->m_recv_over.io_buf;
			unsigned psize = userList[key]->m_curr_packet_size;
			unsigned pr_size = userList[key]->m_prev_packet_data;

			while (io_byte > 0) {
				if (psize == 0) {
					psize = (BYTE)buf[0];
				}
				if (io_byte + pr_size >= psize) {
					unsigned char p[MAX_PACKET_SIZE];
					memcpy(p, userList[key]->io_buf, pr_size);
					memcpy(p + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(key), p);
					io_byte -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0;
					pr_size = 0;
				}
				else {
					memcpy(userList[key]->io_buf + pr_size, buf, io_byte);
					pr_size += io_byte;
					io_byte = 0;
				}
			}
			userList[key]->m_curr_packet_size = psize;
			userList[key]->m_prev_packet_data = pr_size;

			DWORD flags = 0;
			memset(&exover->over, 0, sizeof(WSAOVERLAPPED));
			WSARecv(clientSocket, exover->wsabuf, 1, 0, &flags, &exover->over, 0);
		}
		else {
			delete exover;
		}
	}
}

void LobbyServer::ProcessPacket(int id, void* buf)
{
	char* packet = reinterpret_cast<char*>(buf);
	cout << "ProcessPacket: "<<(int)packet[1] << endl;
	switch (packet[1]) {		//packet[0] = size/ packet[1] = type
	case CL_LOGIN:
	{
		cout << "GET CL_LOGIN" << endl;
		break;
	}
	case CL_DUMMY_LOGIN:
	{
		userList[id]->isActive = true;
		userList[id]->id = id;
		User* newUser = new User();
		newUser->SetPlayerMatch(false);
		newUser->SetPlayerLoginOK("dummy");

		cout << "GET_DUMMY_LOGIN" << endl;

		SendLoginOKPacket(id);
		break;
	}
	case CL_UPDATE_USER_INFO:
	{
		cout << "GET CL_UPDATE_USER_INFO" << endl;
		break;
	}
	}
}

void LobbyServer::SendLoginOKPacket(int id)
{
	lc_packet_login_ok p;
	p.size = sizeof(p);
	p.type = LC_LOGIN_OK;
	p.id = id;

	SendPacket(id, &p);
}

void LobbyServer::SendLoginFailPacket(int id)
{
	lc_packet_login_fail p;
	p.size = sizeof(p);
	p.type = LC_LOGIN_FAIL;

	SendPacket(id, &p);
}

void LobbyServer::SendSignUpOKPacket(int id, int mmr)
{
}

void LobbyServer::SendSignUpFailPacket(int id)
{
}

void LobbyServer::SendMatchPacket(int id, short roomNo, char is_host)
{
}

void LobbyServer::SendUserInfoPacket(int id, int mmr, int winCnt)
{
}

void LobbyServer::SendCancelAutoMatchSuccess(int id)
{
}

void LobbyServer::SendPacket(int id, void* buf)
{
	char* p = reinterpret_cast<char*>(buf);

	EXOVER* sendover = new EXOVER;
	memset(sendover, 0x00, sizeof(EXOVER));
	sendover->is_recv = false;
	memcpy(sendover->io_buf, p, p[0]);
	sendover->wsabuf[0].buf = sendover->io_buf;
	sendover->wsabuf[0].len = p[0];

	DWORD flags = 0;
	int retval = WSASend(userList[id]->m_s, sendover->wsabuf, 1, NULL, flags, &sendover->over, NULL);
	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) error_display("WSASend Error: ", err_no);
	}
}

void LobbyServer::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L"error " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}
