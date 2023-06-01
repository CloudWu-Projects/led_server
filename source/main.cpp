


#include "config.h"
#include <stdio.h>

#define TRACE printf

#include <string>

#include "stringHelper.h"
#include "ledServer.h"
#include "logLib.h"


#include "LED_lsprj.h"
#include <iostream>
#include "logLib.h"
#include <thread>

#include "hv_server.h"
int main(int argc, char* argv[])
{
	auto a = std::filesystem::path(argv[0]);
	auto b = a.remove_filename();

	initLogger(b.string().data());
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaaaaaaa");
	SPDLOG_INFO("a httpPort     ={:6d}   a", IConfig.httpPort);
	SPDLOG_INFO("a ledSDKPort   ={:6d}   a", IConfig.ledSDKPort);
#ifdef NEED_TCPSERVER_FOR_NEIMA
	SPDLOG_INFO("a ledNeiMaPort ={:6d}   a", IConfig.ledNeiMaPort);
#endif
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaaaaaaa");

	
	std::cout << "Current path is " << fs::current_path() << '\n';
	fs::current_path(b);
	std::cout << "Current path is " << fs::current_path() << '\n';



	LED_Server ledServer;

	ledServer.start(IConfig.ledSDKPort);
	std::thread httpThread = std::thread([&]()
		{
			HV_server hvSever;
			hvSever.start(IConfig.httpPort , IConfig.ledSDKPort, IConfig.ledNeiMaPort, &ledServer);
		});


	

	uint64_t loopCnt = 0;
	while (httpThread.joinable())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		printf("%u\r", loopCnt++);
		getchar();
		ledServer.test();
	}
	if (httpThread.joinable())
		httpThread.join();
	getchar();
	return 0;
}