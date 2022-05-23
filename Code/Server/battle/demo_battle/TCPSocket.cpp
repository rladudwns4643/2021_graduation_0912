#include "pch.h"

TCPSocket::~TCPSocket() {	
	/*linger option,
		{0, ����}: ���� ���� ������ �����͸� �����ϰ� ����
		{1, 0}: close()�� �ٷ� ����, ���� �����ʹ� ������, Ŭ�� RST ��Ŷ(������ ����)�� ����
		{1, 1�̻�}: ���ۿ� �����ִ� �����͸� ��� ����, close()�� �ι�° ���ڸ�ŭ �����·� ���, �ι�° ���ڽð� �ȿ� �����͸� ���� �����ϸ� ����, �������� ���ϸ� ������ �Բ� ����*/
	LINGER linger{ 1, 0 };
	setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
}

int TCPSocket::Connect(const SocketAddress& address) {
	int result = ::connect(m_socket, &address.m_sockAddr, address.GetSize());
	if (result == SOCKET_ERROR) {
		SocketUtil::ReportError("TCPSocket::Connect");
		return WSAGetLastError();
	}
	return NO_ERROR;
}

int TCPSocket::Bind(const SocketAddress& address) {
	int result = ::bind(m_socket, &address.m_sockAddr, address.GetSize());
	if (result == SOCKET_ERROR) {
		SocketUtil::ReportError("TCPSocket::Bind");
		return WSAGetLastError();
	}
	return NO_ERROR;
}

int TCPSocket::Listen(int backLog) {
	int result = ::listen(m_socket, backLog);
	if (result == SOCKET_ERROR) {
		SocketUtil::ReportError("TCPSocket::Listen");
		return NO_ERROR;
	}
}

shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& fromAddr) {
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

int TCPSocket::WSASend(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped) {
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

int TCPSocket::WSAReceive(LPWSABUF lpBuf, DWORD dwBufCnt, LPDWORD lpNumBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped)
{
	int bytesRecvd{ ::WSARecv(m_socket, lpBuf, dwBufCnt, lpNumBytesRecvd, dwFlags, lpOverlapped, NULL) };
	if (bytesRecvd == SOCKET_ERROR) {
		int errNum{ WSAGetLastError() };
		if (errNum != WSA_IO_PENDING) {
			SocketUtil::ReportError("TCPSocket::WSAReceive");
			return errNum;
		}
	}
	return bytesRecvd;
}
