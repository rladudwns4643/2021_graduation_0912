#pragma once
#include "Singleton.h"
#include "BattleClient.h"
#include "../../demo_server/demo_server/protocol.h"

#define MAX_BUF 2048
#define MAX_PACKET_SIZE 2048

enum eSERVER { SV_BATTLE, SV_LOBBY, SV_COUNT };

struct SocketInfo {
	SOCKET socket;
	SOCKADDR_IN serverAddr;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;
	bool connect;
};

struct CLIENT {
	int lobby_id{ -1 };
	int battle_id{ -1 };
	bool is_battle_login{ false };
	//lobby, battle
	SocketInfo socket[SV_COUNT]{};

public:
	char id_str[MAX_ID_LEN];
	int mmr{};
};

class NetCore :public TemplateSingleton<NetCore> {
	friend class Service;
public:
	explicit NetCore();
	virtual ~NetCore();

	void Initialize();
	void Destory();

	int GetBattleID();

public:
	void ProcessData(char* buf, size_t io_byte);
	void ProcessPacket(char* packet_buf);

	void CreateSocket(eSERVER sv);
	void ConnectServer(eSERVER sv);
	void DisconnectServer(eSERVER sv);

	int Receive(SOCKET sock, char* buf, int len, int flags);
	void RecvBattleServer();
	void RecvLobbyServer();

	bool SendPacket(void* buf, eSERVER sv);
	void SendLobbyLoginPacket(const std::string& id, const std::string& pw);
	void SendLobbySignUpPacket(const std::string& id, const std::string& pw);
	void SendFindRoomPacket();
	void SendMatchCancelPacket();
	void SendRequestUserInfoPacket();

	bool SendBattleLoginPacket();
	void SendBattleRoomJoinPacket();
	void SendBattleRoomLeavePacket();
	void SendReadyPacket();

	//void SendMovePacket(char key);
	void SendRequestPushAnimPacket(int anim_type);
	void SendRequestPopAnimPacket(int anim_type);
	void SendGetCoinPacket(int coin_id);
	void SendPositionPacket(XMFLOAT3 pos, int anim_type);
	void SendLookVectorPacket(XMFLOAT3& look);
	void SendBulletPacket(XMFLOAT3& dir, short bullet_type);
	void SendRequestDiePacket(int id);

	void SendUpdateUserInfo(int mmr);
	void errorDisplay(const char* msg);
private:
	std::vector<PTC_ROOM> m_recved_room_list{};
	int m_map_info;
	int m_winner;

	CLIENT m_client;

	int m_room_no{};
	bool m_ishost{};

	std::map<int, unique_ptr<BattleClient>> m_battle_clients;
private: //procdata
	unsigned short in_packet_size{ 0 };
	unsigned short saved_packet_size{ 0 };
	char packet_buf[MAX_BUF];
};