#pragma once
#include "pch.h"

class SocketAddress;
class TCPSocket {
public:
	~TCPSocket();

	int Connect(const SocketAddress& address);
	int Bind(const SocketAddress& address);

	int Listen(int backLog = 32);

	std::shared_ptr<TCPSocket> Accept(SocketAddress& fromAddr);

	// Winsock2.h :: WSASend 래핑함수 첫 번째 인자 SOCKET은 멤버 사용, 마지막 인자 COMPLETION_ROUTINE 사용안함.
	int WSASend(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped);

	// Winsock2.h :: WSARecv 래핑함수 첫 번째 인자 SOCKET은 멤버 사용, 마지막 인자 COMPLETION_ROUTINE 사용안함.
	int WSAReceive(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped);

public:
	SOCKET GetSocket() { return m_socket; }
private:
	friend class SocketUtil;
	SOCKET m_socket;
	TCPSocket(SOCKET sock) : m_socket(sock) {};
};

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;