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

LobbyServer::~LobbyServer()
{
	closesocket(listenSocket);
	WSACleanup();
}

void LobbyServer::ClinetAccept(int id)
{
	clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrlen);
	CLIENT* new_user = new CLIENT;
	new_user->id = id;
	new_user->m_s = clientSocket;
	new_user->m_recv_over.wsabuf.len = MAX_BUF_SIZE;
	new_user->m_recv_over.wsabuf.buf = new_user->m_recv_over.io_buf;
	new_user->m_recv_over.is_recv = true;
	new_user->user_info = new User();
	new_user->isActive = true;
	new_user->m_curr_packet_size = 0;
	new_user->m_prev_packet_data = 0;
	
	userList[id] = new_user;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, id, 0);
	memset(&userList[id]->m_recv_over.over, 0, sizeof(WSAOVERLAPPED));
	flags = 0;
	int ret = WSARecv(clientSocket, &userList[id]->m_recv_over.wsabuf, 1, NULL, &flags, &(userList[id]->m_recv_over.over), NULL);
	if (ret != 0) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("WSARecv Error: ", err_no);
	}

	cout << "user: " << id <<"connect"<< endl;
}

void LobbyServer::DoWorker()
{
	while (true) {
		DWORD io_byte;
		ULONG key;
		PULONG p_key = &key;
		WSAOVERLAPPED* p_over;

		BOOL ret = GetQueuedCompletionStatus(iocp, &io_byte, reinterpret_cast<PULONG_PTR>(p_key), &p_over, INFINITE);
		CLIENT& cl = *userList[key];
		SOCKET clientSocket = cl.m_s;

		if (ret == false) {
			int err_no = WSAGetLastError();
			if (err_no == 64) closesocket(clientSocket);
			else error_display("GQCS: ", err_no);
		}

		//클라이언트에서 closesocket 했을 경우
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
			WSARecv(clientSocket, &exover->wsabuf, 1, 0, &flags, &exover->over, 0);
		}
		else {
			delete exover;
		}
	}
}

void LobbyServer::recv_packet_construct(const int& user_id, DWORD& io_byte) {
	//CLIENT& cu = *userList[user_id];
	//EXOVER& r_o = cu.m_recv_over;
	//int rest_byte = io_byte;
	//char* p = userList[user_id]->m_recv_over.io_buf;
	//int packet_size = 0;
	//if (userList[user_id]->m_prev_size != 0) { //남아있는 버퍼에 뭔가 들어있다면
	//	packet_size = userList[user_id]->io_buf[0]; //pakcet_size는 cu.io_buf[0]에 정해져 있음
	//}
	//while (rest_byte > 0) { //처리할 데이터가 남아있다면?
	//	if (packet_size == 0) { //pakcet_size가 지정되어 있지 않다면
	//		packet_size = p[0]; //pakcet_size는 받은 io_buf[0]에 정해져 있음
	//	}
	//	if (packet_size <= rest_byte + userList[user_id]->m_prev_size) { //들어온 패킷 + 남아있는 패킷으로 하나의 패킷이 완성되면
	//		memcpy(userList[user_id]->io_buf + userList[user_id]->m_prev_size, p, packet_size - userList[user_id]->m_prev_size); // 들어온 패킷 + 남아있는 패킷으로 하나의 패킷을 완성함
	//		ProcessPacket(user_id, userList[user_id]->io_buf);
	//		//--패킷 전달 이후 지역변수 초기화--
	//		p += packet_size - userList[user_id]->m_prev_size;
	//		rest_byte -= (packet_size - userList[user_id]->m_prev_size);
	//		packet_size = 0;
	//		userList[user_id]->m_prev_size = 0;
	//		//--끝--
	//	}
	//	else { //들어온 패킷 + 남아있던 패킷으로 하나의 패킷이 완성이 안된다면
	//		memcpy(userList[user_id]->io_buf + userList[user_id]->m_prev_size, p, rest_byte); //io_buf 뒤에 rest_byte 추가 해서 남아있는 패킷에 들어온 패킷을 저장
	//		userList[user_id]->m_prev_size += rest_byte;
	//		rest_byte = 0;
	//		p += rest_byte; //io_buf을 io_buf에 넣은만큼 뒤로 밀어줌
	//	}
	//}
	////process_packet(user_id, r_o.io_buf); //받은 패킷 처리

	char* buf = userList[user_id]->m_recv_over.io_buf;
	unsigned psize = userList[user_id]->m_curr_packet_size;
	unsigned pr_size = userList[user_id]->m_prev_packet_data;

	while (io_byte > 0) {
		if (psize == 0) {
			psize = (BYTE)buf[0];
		}
		if (io_byte + pr_size >= psize) { //패킷완성가능
			unsigned char packet[MAX_PACKET_SIZE];
			memcpy(packet, userList[user_id]->io_buf, pr_size);
			memcpy(packet + pr_size, buf, psize - pr_size);
			ProcessPacket(static_cast<int>(user_id), packet);
			io_byte -= psize - pr_size;
			buf += psize - pr_size;
			psize = 0;
			pr_size = 0;
		}
		else {
			memcpy(userList[user_id]->io_buf + pr_size, buf, io_byte);
			pr_size += io_byte;
			io_byte = 0;
		}
	}
	userList[user_id]->m_curr_packet_size = psize;
	userList[user_id]->m_prev_packet_data = pr_size;
}

void LobbyServer::ProcessPacket(int id, void* buf)
{
	char* packet = reinterpret_cast<char*>(buf);
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
	ZeroMemory(&sendover, sizeof(sendover));
	sendover->is_recv = false;
	memcpy(sendover->io_buf, p, p[0]);
	sendover->wsabuf.buf = sendover->io_buf;
	sendover->wsabuf.len = p[0];

	DWORD flags = 0;
	int retval = WSASend(userList[id]->m_s, &sendover->wsabuf, 1, NULL, flags, &sendover->over, NULL);
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
