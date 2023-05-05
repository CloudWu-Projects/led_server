


#include "config.h"
#include <stdio.h>
#include "LedDll.h"

#define TRACE printf

#include <string>

#include "stringHelper.h"
#include "ledServer.h"
#include "logLib.h"

Config* g_Config = nullptr;

#include "LED_lsprj.h"
#include "httpServer.h"
#include <iostream>
#include "logLib.h"
int main(int argc, char* argv[])
{
	{
		if (1) {
			LED_lsprj ll;
			ll.loadFile(R"(D:\Cloud_wu\LED\led_server\20230427170156.lsprj)");
		}
	}
	initLogger(nullptr);
	g_Config = new Config();
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaa");
	SPDLOG_INFO("a httpPort={:6d}   a", g_Config->httpPort);
	SPDLOG_INFO("a ledPort ={:6d}   a", g_Config->ledPort);
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaa");

	auto a = std::filesystem::path(argv[0]);
	auto b = a.remove_filename();
	std::cout << "Current path is " << fs::current_path() << '\n';
	fs::current_path(b);
	std::cout << "Current path is " << fs::current_path() << '\n';



	LED_Server ledServer;

	ledServer.start(g_Config->ledPort);
	std::thread httpThread = std::thread([&]()
		{
			HttpServer hs;
			hs.startHttpServer(g_Config, ledServer);
		});



	uint64_t loopCnt = 0;
	while (httpThread.joinable())
	{
		Sleep(1000);
		printf("%u\r", loopCnt++);
	}
	if (httpThread.joinable())
		httpThread.join();
	getchar();
	return 0;
}