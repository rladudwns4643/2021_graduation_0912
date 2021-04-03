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

	// Winsock2.h :: WSASend �����Լ� ù ��° ���� SOCKET�� ��� ���, ������ ���� COMPLETION_ROUTINE ������.
	int WSASend(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped);

	// Winsock2.h :: WSARecv �����Լ� ù ��° ���� SOCKET�� ��� ���, ������ ���� COMPLETION_ROUTINE ������.
	int WSAReceive(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped);

public:
	SOCKET GetSocket() { return m_socket; }
private:
	friend class SocketUtil;
	SOCKET m_socket;
	TCPSocket(SOCKET sock) : m_socket(sock) {};
};

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;