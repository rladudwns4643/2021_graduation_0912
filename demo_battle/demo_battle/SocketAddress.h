#pragma once
#include "pch.h"

using namespace std;

class SocketAddress {
public:
	SocketAddress(unsigned int inAddress, unsigned int inPort) {
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetIP4Ref() = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	SocketAddress(const sockaddr& inSockAddr) {
		memcpy(&m_sockAddr, &inSockAddr, sizeof(sockaddr));
	}

	SocketAddress() {
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetIP4Ref() = INADDR_ANY;
		GetAsSockAddrIn()->sin_port = 0;
	}

	bool operator==(const SocketAddress& inOther) const {
		return (m_sockAddr.sa_family == AF_INET 
			&& GetAsSockAddrIn()->sin_port == inOther.GetAsSockAddrIn()->sin_port)
			&& (GetIP4Ref() == inOther.GetIP4Ref());
	}

	// ?
	//size_t GetHash() const {
	//	return (GetIP4Ref()) 
	//		| ((static_cast<unsigned int>(GetAsSockAddrIn()->sin_port)) << 13)
	//		| m_sockAddr.sa_family;
	//}

	int GetSize() const { return sizeof(sockaddr); }

private:
	friend class TCPSocket;

	sockaddr m_sockAddr;
	unsigned int& GetIP4Ref() { return *reinterpret_cast<unsigned int*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
	const unsigned int& GetIP4Ref() const { return *reinterpret_cast<const unsigned int*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }

	sockaddr_in* GetAsSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&m_sockAddr); }
	const sockaddr_in* GetAsSockAddrIn() const { return reinterpret_cast<const sockaddr_in*>(&m_sockAddr); }
};