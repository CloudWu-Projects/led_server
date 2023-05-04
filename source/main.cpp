
#include "httplib.h"
#include <stdio.h>
#include "LedDll.h"

#define TRACE printf

#include <string>
#include <format>

#include "stringHelper.h"
#include "ledServer.h"

void startHttpServer(LED_Server& ledServer,int httpPort)
{
	// HTTP
	httplib::Server svr;

	svr.Get("/hi", [&ledServer](const httplib::Request& req, httplib::Response& res)
		{
			std::wstring sendValue = L"a发送成功aa";


			if (req.has_param("key")) {
				sendValue = to_wide_string(req.get_param_value("key"));				
			}
			auto createRet = ledServer.createAProgram(sendValue);
			std::string htmlContent = std::format("ret:{},msg:{}<br>", std::get<0>(createRet), std::get<1>(createRet));

			
			htmlContent += ledServer.getNetWorkIDList();
			
			res.set_content(htmlContent, "text/html"); 
		});

	svr.listen("0.0.0.0", httpPort);
}

int main()
{
	int httpPort = 8080;
	int ledPort = 10008;
	printf("\naaaaaaaaaaaaaaaaaaaaa");
	printf("\na httpPort=%6d   a",httpPort);
	printf("\na ledPort =%6d   a",ledPort);
	printf("\naaaaaaaaaaaaaaaaaaaaa\n");
	LED_Server ledServer;
	ledServer.start(ledPort);
	std::thread httpThread = std::thread([&]() {
		startHttpServer(ledServer, httpPort);;
		});
	printf("\nbbbbbbbbbbbbbbbbbbbb\n");


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