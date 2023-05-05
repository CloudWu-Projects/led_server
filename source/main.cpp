
#include "httplib.h"

#include "config.h"
#include <stdio.h>
#include "LedDll.h"

#define TRACE printf

#include <string>
#include <format>

#include "stringHelper.h"
#include "ledServer.h"
#include "logLib.h"

Config *g_Config=nullptr;

void startHttpServer(LED_Server& ledServer,int httpPort)
{
	auto SetHandler=
		[&ledServer](const httplib::Request& req, httplib::Response& res)
	{
		std::wstring sendValue = L"test NUll";		
		if (req.method == "POST")
		{
			if (req.get_header_value("Content-Type") != "application/json")
			{
				res.status = 500;
				res.reason = "post with json.";
				return;
			}

			auto a = req.body;
		}
		else
		{
			if (req.has_param("key")) {
				sendValue = to_wide_string(req.get_param_value("key"));

			}
		}
		SPDLOG_DEBUG(L"handle a set key:{}", sendValue);

		auto createRet = ledServer.createAProgram(sendValue, g_Config->ledParam);
		std::string htmlContent = "{";
		htmlContent += std::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));

		htmlContent += ledServer.getNetWorkIDList();
		htmlContent += "}";

		SPDLOG_DEBUG(htmlContent);

		res.set_content(htmlContent, "application/json");
	};
	// HTTP
	httplib::Server svr;
	svr
		.Get("/", [&ledServer](const httplib::Request& req, httplib::Response& res)
			{
				std::string htmlContent;
				htmlContent = "{" + ledServer.getNetWorkIDList() + "}";
				res.set_content(htmlContent, "application/json");
			})
		.Get("/set", SetHandler)
		.Post("/set", SetHandler);



	svr.listen("0.0.0.0", httpPort);
}
int main(int argc,char*argv[])
{
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
	std::thread httpThread = std::thread([&]() {
		startHttpServer(ledServer, g_Config->httpPort);;
		});
	


	uint64_t loopCnt = 0;
	while (httpThread.joinable())
	{
		Sleep(1000);
		printf("%u\r", loopCnt++);
	}
	if(httpThread.joinable())
		httpThread.join();
	getchar();
	return 0;
}