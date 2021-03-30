#pragma once
#include "pch.h"

class SocketAddress;
class SocketUtil;
class TCPSocket {
public:
	~TCPSocket() {
		/*linger option, 
		{0, ����}: ���� ���� ������ �����͸� �����ϰ� ����
		{1, 0}: close()�� �ٷ� ����, ���� �����ʹ� ������, Ŭ�� RST ��Ŷ(������ ����)�� ����
		{1, 1�̻�}: ���ۿ� �����ִ� �����͸� ��� ����, close()�� �ι�° ���ڸ�ŭ �����·� ���, �ι�° ���ڽð� �ȿ� �����͸� ���� �����ϸ� ����, �������� ���ϸ� ������ �Բ� ����*/
		LINGER linger{ 1,0 };
		setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
		shutdown(m_socket, SD_SEND);
		closesocket(m_socket);
	}

	int Connect(const SocketAddress& address) {
		int retval = ::connect(m_socket, &address.m_sockAddr, address.GetSize());
		if (retval == SOCKET_ERROR) {
			SocketUtil::ReportError("TCPSocket::Connect");
			return WSAGetLastError();
		}
		return NO_ERROR;
	}

	int Bind(const SocketAddress& address) {
		int retval = ::bind(m_socket, &address.m_sockAddr, address.GetSize());
		if (retval = SOCKET_ERROR) {
			SocketUtil::ReportError("TCPSocket::Bind");
			return WSAGetLastError();
		}
		return NO_ERROR;
	}

	int Listen(int backLog = 32) {
		int retval = ::listen(m_socket, backLog);
		if (retval == SOCKET_ERROR) {
			SocketUtil::ReportError("TCPSocket::Listen");
			return WSAGetLastError();
		}
		return NO_ERROR;
	}

	std::shared_ptr<TCPSocket> Accept(SocketAddress& fromAddr) {
		int length{ static_cast<int>(fromAddr.GetSize()) };
		SOCKET newSocket = ::WSAAccept(m_socket, &fromAddr.m_sockAddr, &length, NULL, NULL);
		if (newSocket != INVALID_SOCKET) {
#ifdef LOG_ON
			std::cout << "TCP SOCKET CREATED\n";
#endif
			return TCPSocketPtr(new TCPSocket(newSocket));
		}
		else return nullptr;
	}

	// Winsock2.h :: WSASend �����Լ� ù ��° ���� SOCKET�� ��� ���, ������ ���� COMPLETION_ROUTINE ������.
	int WSASend(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped) {
		if (this == nullptr) {
#ifdef LOG_ON
			std::cout << "WSASend nullptr references\n";
#endif
			return -1;
		}

		int retval = ::WSASend(m_socket, lpBuf, dwBufCnt, lpNumBytesSent, dwFlags, lpOverlapped, NULL);
		if (retval == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING) {
				SocketUtil::ReportError("TCPSocket::WSASend");
				return err;
			}
		}
		return retval;
	}

	// Winsock2.h :: WSARecv �����Լ� ù ��° ���� SOCKET�� ��� ���, ������ ���� COMPLETION_ROUTINE ������.
	int WSAReceive(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped) {
		int retval = ::WSARecv(m_socket, lpBuf, dwBufCnt, lpNumBytesRecvd, dwFlags, lpOverlapped, NULL);
		if (retval == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING) {
				SocketUtil::ReportError("TCPSocket::WSARecv");
				return err;
			}
		}
		return retval;
	}

public:
	SOCKET GetSocket() { return m_socket; }
private:
	friend class SocketUtil;
	SOCKET m_socket;
	TCPSocket(SOCKET sock) : m_socket(sock) {};
};

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;