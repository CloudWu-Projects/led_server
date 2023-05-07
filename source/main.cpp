


#include "config.h"
#include <stdio.h>
#include "LedDll.h"

#define TRACE printf

#include <string>

#include "stringHelper.h"
#include "ledServer.h"
#include "logLib.h"


#include "LED_lsprj.h"
#include "httpServer.h"
#include <iostream>
#include "logLib.h"
#include <thread>
int main(int argc, char* argv[])
{
	initLogger(nullptr);
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaa");
	SPDLOG_INFO("a httpPort={:6d}   a", IConfig.httpPort);
	SPDLOG_INFO("a ledPort ={:6d}   a", IConfig.ledPort);
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaa");

	auto a = std::filesystem::path(argv[0]);
	auto b = a.remove_filename();
	std::cout << "Current path is " << fs::current_path() << '\n';
	fs::current_path(b);
	std::cout << "Current path is " << fs::current_path() << '\n';



	LED_Server ledServer;

	ledServer.start(IConfig.ledPort);
	std::thread httpThread = std::thread([&]()
		{
			HttpServer hs;
			hs.startHttpServer( ledServer);
		});



	uint64_t loopCnt = 0;
	while (httpThread.joinable())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		printf("%u\r", loopCnt++);
	}
	if (httpThread.joinable())
		httpThread.join();
	getchar();
	return 0;
}