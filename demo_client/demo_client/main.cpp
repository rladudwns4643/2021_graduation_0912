#include "pch.h"
#include "Dummy.h"

Dummy* g_dummy = NULL;

int main()
{
	g_dummy = new Dummy();

	boost::asio::io_context io;
	boost::asio::deadline_timer timer(io, boost::posix_time::millisec(50));
	timer.async_wait(boost::bind(&Dummy::ConnectLobbyServer, g_dummy, boost::asio::placeholders::error, &timer));

	std::thread worker_thread(&Dummy::DoWorker, g_dummy);

	worker_thread.join();
	delete g_dummy;
}