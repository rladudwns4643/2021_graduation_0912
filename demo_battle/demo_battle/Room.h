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

#pragma region Update
	void Update(); //Battle World Update
	void PushMsg(message&);
	void AnimationUpdate(float elapsedMilliSec);
	void WorldUpdate();
#pragma endregion

public:
	bool IsEmpty(); //is Room Empty?
	bool EnterRoom(int id, bool is_roomMnr);
	void AnnounceRoomEnter(int id);
	void LeaveRoom(int id);
	PTC_VECTOR GetCoinRandPos();

	std::wstring GetRoomName();
	int GetcurPlayerNum();
	int GetRoomNo();

	bool IsGameStarted();
	bool IsRoomEnterable() { return m_isEnterable; }
	bool IsRoomStarted();

	void CreateAddCoinEvent();
	void AddCoinByDie(int die_id);

	void CreateReloadBulletEvent();
	void SendLeftTimePacket();

	void CheckGameState();
	void GameStart();
	void RoundStart();
	void GameOver(int winner);

	void UpdateUserInfo_DB(int winner);


public:		//send msg
#pragma region Push Msg
	void PushSendMsg(int id, void* buff);
	void FlushSendMsg();

	void PushPlayerPositionMsg(int to, int from, PTC_VECTOR* position_info);
	void PushPlayerDirectionMsg(int to, int from, PTC_VECTOR look);
	void PushObjectPositionMsg(int id, short type_id, int obj_id, PTC_VECTOR* position_info);
	void PushShootBulletMsg(int from, short bullet_type, short bullet_idx, PTC_VECTOR look, PTC_VECTOR pos);
	void PushRemoveBulletMsg(int bullet_id);
	void PushCallBackHitMsg(int be_hit_id);
	void PushDieMsg(int die_id);
	void PushAddCoinMsg(PTC_VECTOR coin_pos, int coin_id);
	void PushUpdateCoinMsg(int update_id, int update_cnt, int delete_coin_id);
	void PushNewWinSatisfaction(int satisfaction_id);
	void PushGameOverMsg(int winner_id);
	void PushSendTimeMsg(char left_time);
	void PushReloadBulletMsg();
	void PushPlayerLookMsg(int to, int from, PTC_VECTOR look);
	void PushRoundStartMsg();
	void PushRoomLeaveMsg(int to, int leave);
	void PushGameStartMsg(int id, PTC_START_INFO* player_info);
	void PushRespawnMsg();

	void PushReadyMsg(int id, bool ready);
	void PushUnReadyMsg(int id);
	void PushRoomStartAvailableMsg(int id, bool available);

	void PushNewRoomMnrMsg(int id);

	void PushAnimPushMsg(int to, int from, int animType);
	void PushAnimPopMsg(int to, int from, int animType);
#pragma endregion
	
	//set user move type
	void MakeMove(int id);
	void MakeStop(int id);

	int GetEmptyBullet();
	void SetEmptyBullet();

	void Respawn();

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


#pragma region objects
	std::array<Player*, MAX_PLAYER> m_players;
	std::atomic_int	m_curPlayerNum;

	std::array<bool, MAX_BULLET> m_bullets;
	std::queue<int> m_q_bullet;

	std::array<bool, MAX_COIN> m_coins;
	atomic_int m_coin_cur{ 0 };
#pragma endregion

#pragma region msg Queue
	std::queue<message> m_recvMsgQueue;
	std::queue<message> m_copiedRecvMsgs;
	std::mutex			m_recvMsgQueueLock;

	std::queue<SEND_INFO> m_sendMsgQueue;
	std::mutex m_sendMsgQueueLock;
#pragma endregion
	
	//Physics* m_physics = nullptr;		//충돌 처리 용
	Map* m_map = nullptr;

	std::chrono::system_clock::time_point m_lastUpdate;
	std::chrono::system_clock::time_point m_curUpdate;
	std::chrono::duration<float> m_elapsedSec;
};