#pragma once
#include "TCPSocket.h"

enum SocketAddressFamily {
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil {
public:
	static bool StaticInit() {
		WSADATA wsa;
		int retval = WSAStartup(MAKEWORD(2, 2), &wsa);
		if (retval != NO_ERROR) {
			ReportError("SocketUtil - init");
			return false;
		}
		return true;
	}

	static void CleanUp() {
		WSACleanup();
	}
	
	static void ReportError(const char* operationDesc) {
		WCHAR* lpMsgBuf;
		DWORD errorNumber = WSAGetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorNumber,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		std::wcout << L"Error " << errorNumber << " - " << operationDesc << ", " << lpMsgBuf << std::endl;
	}

	static TCPSocketPtr CreateTCPSocket(SocketAddressFamily family) {
		SOCKET s = WSASocket(family, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (s != INVALID_SOCKET) {
#ifdef LOG_ON
			std::cout << "TCP Socket Created" << std::endl;
#endif
			return TCPSocketPtr(new TCPSocket(s));
		}
		else {
			ReportError("SocketUtil::CreateTCPSocket");
			return nullptr;
		}
	}
};