
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

void startHttpServer(LED_Server& ledServer,int httpPort)
{
	// HTTP
	httplib::Server svr;
svr.Get("/", [&ledServer](const httplib::Request& req, httplib::Response& res)
		{
			std::string htmlContent ;
			htmlContent += ledServer.getNetWorkIDList();			
			res.set_content(htmlContent, "text/html"); 
		});
	

	svr.Get("/set", [&ledServer](const httplib::Request& req, httplib::Response& res)
		{
			std::wstring sendValue = L"test NUll";
			if (req.has_param("key")) {
				sendValue = to_wide_string(req.get_param_value("key"));				
				
			}
			SPDLOG_DEBUG(L"handle a set key:{}", sendValue);

			auto createRet = ledServer.createAProgram(sendValue);
			std::string htmlContent = std::format("ret:{},msg:{}<br>", std::get<0>(createRet), std::get<1>(createRet));
		
			htmlContent += ledServer.getNetWorkIDList();

			SPDLOG_DEBUG(htmlContent);

			res.set_content(htmlContent, "text/html"); 
		});
	

	svr.listen("0.0.0.0", httpPort);
}
int main(int argc,char*argv[])
{
	initLogger(nullptr);

	Config c;
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaa");
	SPDLOG_INFO("a httpPort={:6d}   a",c.httpPort);
	SPDLOG_INFO("a ledPort ={:6d}   a",c.ledPort);
	SPDLOG_INFO("aaaaaaaaaaaaaaaaaaaa");
	
	auto a = std::filesystem::path(argv[0]);
	auto b = a.remove_filename();
	std::cout << "Current path is " << fs::current_path() << '\n'; 	
	fs::current_path(b);	
	std::cout << "Current path is " << fs::current_path() << '\n';
	
	

	LED_Server ledServer;
	ledServer.start(c.ledPort);
	std::thread httpThread = std::thread([&]() {
		startHttpServer(ledServer, c.httpPort);;
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