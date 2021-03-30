#pragma once
#include "pch.h"

class SocketAddress;
class SocketUtil;
class TCPSocket {
public:
	~TCPSocket() {
		/*linger option, 
		{0, 무관}: 남은 소켓 버퍼의 데이터를 전송하고 종료
		{1, 0}: close()는 바로 리턴, 남은 데이터는 버려짐, 클라에 RST 패킷(리셋을 위한)을 보냄
		{1, 1이상}: 버퍼에 남아있는 데이터를 모두 전송, close()는 두번째 인자만큼 블럭상태로 대기, 두번째 인자시간 안에 데이터를 전부 전송하면 리턴, 전송하지 못하면 에러와 함께 리턴*/
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

	// Winsock2.h :: WSASend 래핑함수 첫 번째 인자 SOCKET은 멤버 사용, 마지막 인자 COMPLETION_ROUTINE 사용안함.
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

	// Winsock2.h :: WSARecv 래핑함수 첫 번째 인자 SOCKET은 멤버 사용, 마지막 인자 COMPLETION_ROUTINE 사용안함.
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