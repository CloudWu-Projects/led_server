
#include "httpServerWrapper.h"

#include "config.h"
#include <stdio.h>
#include "LedDll.h"

#define TRACE printf

#include <string>

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
		htmlContent += fmt::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));
		htmlContent += fmt::format("\"sendValue\":\"{}\",", to_byte_string( sendValue));
		htmlContent += ledServer.getNetWorkIDList();
		htmlContent += "}";

		SPDLOG_DEBUG(htmlContent);

		res.set_content(htmlContent, "application/json");
	};
	std::string htmlContent;
	htmlContent = fmt::format("\"sendValue\":\"{}\",", "aaa");
	// HTTP
	httplib::HttpServerWrapper svr;
	svr
		.Get("/list", [&ledServer](const httplib::Request& req, httplib::Response& res)
			{
				std::string htmlContent;
				htmlContent = "{" + ledServer.getNetWorkIDList() + "}";
				res.set_content(htmlContent, "application/json");
			})
		.Get("/set", SetHandler)
		.Post("/set", SetHandler);



	svr.listen("0.0.0.0", httpPort);
}
#include "tinyxml2.h"
int main(int argc,char*argv[])
{
	{
		if(0){
			tinyxml2::XMLDocument doc;
			doc.LoadFile(R"(D:\Cloud_wu\LED\led_server\20230427170156.lsprj)");

			// Structure of the XML file:
			// - Element "PLAY"      the root Element, which is the
			//                       FirstChildElement of the Document
			// - - Element "TITLE"   child of the root PLAY Element
			// - - - Text            child of the TITLE Element

			// Navigate to the title, using the convenience function,
			// with a dangerous lack of error checking.
			const char* title = doc.FirstChildElement("PLAY")->FirstChildElement("TITLE")->GetText();
			printf("Name of play (1): %s\n", title);

			// Text is just another Node to TinyXML-2. The more
			// general way to get to the XMLText:
			tinyxml2::XMLText* textNode = doc.FirstChildElement("PLAY")->FirstChildElement("TITLE")->FirstChild()->ToText();
			title = textNode->Value();
			printf("Name of play (2): %s\n", title);
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