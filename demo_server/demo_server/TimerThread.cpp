#include "pch.h"
#include "protocol.h"
#include "TimerThread.h"

void TimerThread::InitThread() {
	mythread = std::thread([&]() {TimerThread::ProcThread(); });
}

void TimerThread::ProcThread() {
	while (true) {
		ATOMIC::g_match_list_lock.lock();
		while (SR::g_match_list.empty()) {
			ATOMIC::g_match_list_lock.unlock();
			this_thread::sleep_for(10ms);
			ATOMIC::g_match_list_lock.lock();
		}
		MatchingUser();
		ATOMIC::g_match_list_lock.unlock();

	}
}

void TimerThread::JoinThread()
{
	mythread.join();
}

bool TimerThread::MatchingUser()
{
	const auto& now{ chrono::high_resolution_clock::now() };
	int mmr_soft_gap{ 0 };
	int cur_id;
	int cur_mmr;
	for (const auto& u : SR::g_match_list) {
		cur_id = u.first;
		cur_mmr = u.second.mmr;
		mmr_soft_gap = chrono::duration_cast<chrono::milliseconds>(now - u.second.wait_time).count();
		for (const auto& u : SR::g_match_list) {
			if (u.first == cur_id) continue;
			if (abs(u.second.mmr - cur_mmr) < mmr_soft_gap + mmr_hard_gap) {
				SendRequestRoomPacket(cur_id, u.first);
				SR::g_match_list.erase(cur_id);
				SR::g_match_list.erase(u.first);
				return true;
			}
		}
	}
	return false;
}
void TimerThread::SendRequestRoomPacket(int id_1, int id_2) {
	lb_packet_request_room p;
	p.size = sizeof(p);
	p.type = LB_REQUEST_ROOM;
	p.id_1 = id_1;
	p.id_2 = id_2;

	SendPacket(&p); //battle server·Î
}

void TimerThread::SendPacket(void* buf) {
	char* p = reinterpret_cast<char*>(buf);

	EXOVER* sendover = new EXOVER;
	memset(sendover, 0x00, sizeof(EXOVER));
	sendover->is_recv = false;
	memcpy(sendover->io_buf, p, p[0]);
	sendover->wsabuf[0].buf = sendover->io_buf;
	sendover->wsabuf[0].len = p[0];

	cout << "Send ID: 0 Packet: " << (int)p[1] << endl;

	DWORD flags = 0;
	int retval = WSASend(SR::g_battleSocket, sendover->wsabuf, 1, NULL, flags, &sendover->over, NULL);
	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) assert(!"TimerThread: Send");
	}
}