#include "pch.h"
#include "TimerThread.h"
#include "extern.h"

void TimerThread::InitThread() {
	mythread = std::thread([&]() {TimerThread::ProcThread(); });
}


void TimerThread::ProcThread() {
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

		EX_OVER* ex_over = new EX_OVER;
		if (SR::g_rooms[proc_ev.target] != NULL) {
			switch (proc_ev.et) {
			case EVENT_TYPE::EV_UPDATE: {
				ex_over->ev_type = EVENT_TYPE::EV_UPDATE;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_TICK: {
				ex_over->ev_type = EVENT_TYPE::EV_TICK;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_FLUSH_MSG: {
				ex_over->ev_type = EVENT_TYPE::EV_FLUSH_MSG;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_ADD_COIN: {
				ex_over->ev_type = EVENT_TYPE::EV_ADD_COIN;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_MOVE_ENABLE: {
				ex_over->ev_type = EVENT_TYPE::EV_MOVE_ENABLE;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_MAKE_MOVE_DISABLE: {
				ex_over->ev_type = EVENT_TYPE::EV_MAKE_MOVE_DISABLE;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_UPDATE_DB: { //아직없음
				//ex_over->ev_type = EV_UPDATE_DB;
				break;
			}
			case EVENT_TYPE::EV_RESET_ROOM: {
				ex_over->ev_type = EVENT_TYPE::EV_RESET_ROOM;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			case EVENT_TYPE::EV_WORLD_UPDATE: {
				ex_over->ev_type = EVENT_TYPE::EV_WORLD_UPDATE;
				*(int*)ex_over->net_buf = proc_ev.target;
				break;
			}
			default: {
#ifdef LOG_ON
				std::cout << "Unknown Event Type Error\n";
#endif
				while (true);
				break;
			}
			}
			//GetQueuedCompletionStatus에 event 내용 전달
			bool retval = PostQueuedCompletionStatus(SR::g_iocp, 1, proc_ev.id, &ex_over->over);
		}
		else {
#ifdef LOG_ON
			std::cout << "g_rooms[proc_ev.target] is NULL\n";
#endif
		}
	}
}

void TimerThread::JoinThread() {
	mythread.join();
}