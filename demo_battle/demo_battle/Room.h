#pragma once
#include "../../demo_server/demo_server/protocol.h"
#include "Player.h"
#include "Global.h"
#include "battleServer.h"
#include "Bullet.h"
#include "Map.h"
#include "extern.h"
#include "Toy.h"
#include "Boundary.h"
//#include "Physics.h"

//class Physics;
class Map;
class Room {
public:
	explicit Room(int room_no);
	explicit Room(int room_no, wstring room_name);
	~Room();

public:
	void Initialize(int room_no);
	void Reset();
	void Update(); //Battle World Update
	void PushMsg(message&);
	void Collision();
	void WorldUpdate();

public:
	bool IsEmpty(); //is Room Empty?
	bool EnterRoom(int id, bool is_roomMnr);
	void AnnounceRoomEnter(int id);
	void LeaveRoom(int id);

	void LoadMap(); //맵 하나
	void UpdateActive();
	std::wstring GetRoomName();
	int GetcurPlayerNum();
	int GetRoomNo();

	bool IsGameStarted();
	bool IsRoomEnterable() { return m_isEnterable; }
	bool IsRoomStarted();

	void SendLeftTimePacket();

	void CheckGameState();
	void GameStart();
	void RoundStart();
	void GameOver(int winner_role);

	void UpdateUserInfo_DB(int winner_role);


public:		//send msg
	void PushSendMsg(int id, void* buff);
	void FlushSendMsg();

	void PushPlayerPositionMsg(int to, int from, PTC_VECTOR* position_info);
	void PushPlayerDirectionMsg(int to, int from, PTC_VECTOR look);
	void PushObjectPositionMsg(int id, short type_id, int obj_id, PTC_VECTOR* position_info);
	void PushTransformMsg(int transformer, short obj_type);
	void PushShootBulletMsg(int to, PTC_VECTOR look);
	void PushHitMsg(int hit_id, int dmg);
	void PushDieMsg(int die_id);

	void PushReadyMsg(int id);
	void PushUnReadyMsg(int id);
	void PushGameStartAvailableMsg(int id, bool available);

	void PushNewRoomMnrMsg(int id);

	void PushAnimMsg(int id, int animType);

public:		//
	void MakeMove(int id);
	void MakeStop(int id);


private:	//recv msg
	void CopyRecvMsgs();		//m_msg에 msg push
	void ClearCopyMsg();
	void ProcMsg(message msg); // msg 처리


private:
	bool m_isGameStarted;
	bool m_isEnterable{ true };
	bool m_isRoundStarted;
	bool m_isSent{ false }; // host에게 bc_packet_gamestart_available 전송 후 true
	int m_roomNo;
	std::wstring m_roomName;
	int m_RoomMnr;

	unsigned char m_leftTime;
	std::atomic_int m_countFrame{ 0 };

	std::atomic_int	m_curPlayerNum;
	short			m_ToyNum;

	std::array<Player*, MAX_PLAYER> m_players;
	std::array<Bullet, MAX_BULLET> m_bullets;

	std::queue<message> m_recvMsgQueue;
	std::queue<message> m_copiedRecvMsgs;
	std::mutex m_recvMsgQueueLock;

	std::queue<SEND_INFO> m_sendMsgQueue;
	std::mutex m_sendMsgQueueLock;

	//Physics* m_physics = nullptr;		//충돌 처리 용
	Map* m_map = nullptr;

	std::chrono::system_clock::time_point m_lastUpdate;
	std::chrono::system_clock::time_point m_curUpdate;
	std::chrono::duration<float> m_elapsedSec;
};