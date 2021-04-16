#pragma once
#include "Global.h"
#include "extern.h"
#include "MyThread.h"

class TimerThread final : public MyThread {
public:
	void InitThread() override {
		mythread = std::thread([&]() {TimerThread::ProcThread(); });
	}

	void ProcThread() override {
		while (true) {
			ATOMIC::g_timer_lock.lock();
			while (SR::g_timer_queue.empty()) {
				ATOMIC::g_timer_lock.unlock();
				this_thread::sleep_for(10ms);
				ATOMIC::g_timer_lock.lock();
			}

			const EVENT& ev = SR::g_timer_queue.top(); //가장 오래 기다린 event
			if (chrono::high_resolution_clock::now() < ev.event_time) { //현 시간보다 eventTime이 뒤라면(아직 처리하면 안되는 event라면)
				ATOMIC::g_timer_lock.unlock();
				this_thread::sleep_for(1ms);
				continue;
			}
			EVENT proc_ev = ev;
			SR::g_timer_queue.pop();
			ATOMIC::g_timer_lock.unlock();

//			EX_OVER* ex_over = new EX_OVER;
//			if (SHARED_RESOURCE::g_rooms[proc_ev.target] != NULL) {
//				ex_over->ev_type = proc_ev.et;
//				*(int*)ex_over->net_buf = proc_ev.target;
//				/*
//				switch (proc_ev.et) {
//				case EV_UPDATE: {
//					break;
//				}
//				case EV_TICK: {
//					break;
//				}
//				case EV_MOVE_ENABLE: {
//					break;
//				}
//				case EV_MAKE_MOVE_DISABLE: {
//					break;
//				}
//				case EV_UPDATE_DB: {
//					break;
//				}
//				case EV_RESET_ROOM: {
//					break;
//				}
//				case EV_WORLD_UPDATE: {
//					break;
//				}
//				default: {
//#ifdef LOG_ON
//					std::cout << "Unknown Event Type Error\n";
//#endif
//					while (true);
//					break;
//				}
//				}*/
//				//GetQueuedCompletionStatus에 event 내용 전달
//				bool retval = PostQueuedCompletionStatus(SHARED_RESOURCE::g_iocp, 1, proc_ev.id, &ex_over->over);
//			}
//			else {
//#ifdef LOG_ON
//				std::cout << "g_rooms[proc_ev.target] is NULL\n";
//#endif
//			}
		}
	}

	void JoinThread() override {
		mythread.join();
	}
};