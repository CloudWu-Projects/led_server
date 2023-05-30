#include "HV_serverImp.h"
#include "hv_server.h"


#include <iostream>
#include "ledServer.h"

#include "stringHelper.h"

int HV_server::start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer)
{
	Imp = new HV_serverImp();

	return Imp->start(httpPort, ledSDKport,ledNeimaPort, ledServer);

}

#define TEST_TLS        0
static const	char updateAEnd[4] = { 0x00, 0x00, 0x0D, 0x0A };
static int makeNeiMa(char* pDestbuffer, std::vector<std::string> pDataVt)
{
	uint8_t updateAHeader[] = { 0x55, 0xAA, 0x00, 0x00, 0x01, 0x01, 0x00, 0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	int send_len = 0;
	int recv_len = 0;
	//���巢�ͻ������ͽ��ܻ�����
	//char send_buf[1024];
	auto pgetInterCode = [](char* destPos, int codeAreaIdx, std::string str)
	{
		uint8_t InterCodeHeader[] = { 0x25, 0x64, 0x69, 0x73, 0x70, 0x30, 0x3A, 0x30 };
		InterCodeHeader[5] = 0x30 + codeAreaIdx;
		memcpy(destPos, InterCodeHeader, sizeof(InterCodeHeader));
		destPos += sizeof(InterCodeHeader);
		memcpy(destPos, str.c_str(), str.length());
		return destPos + str.length();
	};
	char* pPos = pDestbuffer;
	memcpy(pPos, updateAHeader, sizeof(updateAHeader));
	pPos += sizeof(updateAHeader);
	for (int i=0;i<pDataVt.size();i++)
	{
		pPos = pgetInterCode(pPos, i,pDataVt[i]);
	}
	memcpy(pPos, updateAEnd, sizeof(updateAEnd));
	pPos += sizeof(updateAEnd);
	int nLen = pPos - pDestbuffer;

	return nLen;
}


int HV_serverImp::start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer)
{
	m_ledSever = ledServer;
	std::thread a = std::thread([&]() 
		{
			tcp_server(ledNeimaPort, ledSDKport);
		});
		
#ifdef use_cinatra
	start_http_server(httpPort);
#endif
	a.join();
	return 0;
}

#ifdef use_cinatra
inline int HV_serverImp::list_Handler(HttpRequest& req, HttpResponse& res) {
	auto s = m_ledSever->getNetWorkIDList();
	//m_tcpSrv.foreachChannel([&](auto channel) {
	//	if (channel->isConnected()) {
	//		std::string peeraddr = channel->peeraddr();
	//		s += fmt::format(">> {}", channel->peeraddr());
	//		}
	//	});

	res.set_status_and_content(status_type::ok, "s");
	return 0;
}

inline int HV_serverImp::CreatePGM_Handler(HttpRequest& req, HttpResponse& res)
{
	
	ExtSeting extSetting;
	std::string sendValue = "test NUll";
	bool isJson = false;
	if (req.get_method() == "POST")
	{
		if (req.get_header_value("Content-Type") != "application/json")
		{
			//	res.status = 500;
			//	res.reason = "post with json.";
			//	return;
		}

		/*
		{
		}
		*/
		//sendValue = req.get_();
		isJson = true;
	}
	else
	{
	}

	{
		sendValue = req.get_query_value("content");
		if (sendValue.empty())
			sendValue = "content is null";
	}
	auto fontSize = req.get_query_value("fontsize");
	if (!fontSize .empty())
	{
		extSetting.FontSize = atoi(fontSize.data());
	}
#ifdef UNICODE
	SPDLOG_DEBUG(L"handle a set key:{}", sendValue);
#else
	SPDLOG_DEBUG("handle a set key:{}", sendValue);
#endif
	auto url = req.get_url();
	std::string htmlContent = "{";
	auto createRet = (req.get_url() == "/create_onePGM") ?
		m_ledSever->create_onPGM_byCode(sendValue, extSetting)
		: m_ledSever->createPGM_withLspj(isJson, sendValue, extSetting);

	htmlContent += fmt::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));
	htmlContent += fmt::format("\"sendValue\":\"{}\",", (sendValue));
	htmlContent += m_ledSever->getNetWorkIDList();
	htmlContent += "}";
	/**/
	SPDLOG_DEBUG(htmlContent);
	
	//req_content_type::string
	res.set_status_and_content(status_type::ok, std::move(htmlContent),req_content_type::json);
	return 0;
	
}

inline int HV_serverImp::start_http_server(int httpPort)
{
	
	//httpServer->Static("/","./webPage");
	httpServer = new http_server(std::thread::hardware_concurrency());
	auto fport = fmt::format("{}", httpPort);
	httpServer->listen("0.0.0.0",fport);

	httpServer->set_http_handler<POST>("/leds/add", [this](HttpRequest& req, HttpResponse&  res) {
		std::string htmlContent;
			htmlContent = "{";
			htmlContent += true ? "sucess" : "failed";
			htmlContent += "}";
			res.set_status_and_content(status_type::ok, std::move(htmlContent), req_content_type::json);
	});
	httpServer->set_http_handler<GET>("/leds/list", [this](HttpRequest& req, HttpResponse&  res) {
		 list_Handler(req, res);
		});
	httpServer->set_http_handler<GET>("/list", [this](HttpRequest& req, HttpResponse&  res) {
		 list_Handler(req, res);
		});

	httpServer->set_http_handler<GET>("/paths", [this](HttpRequest& req, HttpResponse&  res) {
		//return res.Json(httpServer->Paths());

		res.set_status_and_content(status_type::ok, "");
		});

	httpServer->set_http_handler<POST>("/set", [this](HttpRequest& req, HttpResponse&  res) {
		 CreatePGM_Handler(req, res);
		}); ;

	httpServer->set_http_handler<GET>("/create_onePGM", [this](HttpRequest& req, HttpResponse&  res) {
		 CreatePGM_Handler(req, res);
		});

	httpServer->set_http_handler<GET>("/create_withPGM", [this](HttpRequest& req, HttpResponse&  res) {
		 CreatePGM_Handler(req, res);
		});

	httpServer->set_http_handler<GET>("/reloadpgm", [this](HttpRequest& req, HttpResponse&  res)
		{
			
			std::string 	sendValue = (std::string)req.get_query_value("key");
			std::string htmlContent;
			htmlContent = "{";
			htmlContent += (IConfig.ReloadPGM(std::move(sendValue)) ? "sucess" : "failed");
			htmlContent += "}";
			res.set_status_and_content(status_type::ok, std::move(htmlContent), req_content_type::json);
		});

	httpServer->set_http_handler<GET>("/neima", [this](HttpRequest& req, HttpResponse&  res) {
		if (auto key = req.get_query_value("key"); !key.empty())
		{
			std::lock_guard<std::mutex> lock(led_neima_mutex);
			auto v = split_string(key);
			int nlen = makeNeiMa(neima, v);
			int a = 0;
			int count = tcpServer.boardCast(neima, nlen);
			std::string htmlJson;
			htmlJson =fmt::format("{{\"ret\":{} ,\"ledCount\":{} ,\"msg\":\"{}\"}}",a,count,"ok");

			res.set_status_and_content(status_type::ok, std::move(htmlJson), req_content_type::json);
		}
		res.set_status_and_content(status_type::ok, fmt::format("{{\"ret\":-1 ,\"ledCount\":0 ,\"msg\":\"miss key\"}}"), req_content_type::json);
		});


	printf("http:%d\n", httpPort);
	httpServer->run();
	return 0;
}

	#endif
inline int HV_serverImp::tcp_server(int _proxy_port, int _backend_port) {
	
	tcpServer.start(fmt::format("{}",_proxy_port).data(),
		fmt::format("{}", _backend_port).data());
	return 0;
}
