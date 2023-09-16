#include "HV_serverImp.h"
#include "hv_server.h"



#include <iostream>
#include "ledServer.h"

#include "stringHelper.h"

int HV_server::start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer)
{
	Imp = new HV_serverImp();

	return Imp->start(httpPort, ledSDKport, ledNeimaPort, ledServer);
}

int HV_serverImp::start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer)
{
	m_ledSever = ledServer;
#ifdef NEED_TCPSERVER_FOR_NEIMA
	tcp_server(ledNeimaPort, ledSDKport);
#endif
	http_server(httpPort);
	return 0;
}
inline int HV_serverImp::list_Handler(const httplib::Request& req, httplib::Response& resp)
{
	auto s = m_ledSever->getNetWorkIDList();
	nlohmann::json js;
	time_t t = time(NULL);
	js["haha"] = t;
	js["list"] = s; 
	resp.set_content(js.dump(4), "application/json");
	return 0;
}

inline int HV_serverImp::CreatePGM_Handler(const httplib::Request& req, httplib::Response& res)
{
	res.set_header("Access-Control-Allow-Origin", "*");
	ExtSeting extSetting;
	std::string sendValue = "test NUll";
	bool isJson = false;

	if (req.method == "POST")
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
		sendValue = req.body;
		isJson = true;
	}
	else
	{
	}
	//
	//	{
	//		sendValue = req->GetParam("content");
	//	}
	//	auto fontSize = req->GetParam("fontsize", "-1");
	//	if (fontSize != "-1")
	//	{
	//		extSetting.FontSize = atoi(fontSize.data());
	//	}
	//#ifdef UNICODE
	//	SPDLOG_DEBUG(L"handle a set key:{}", sendValue);
	//#else
	//	SPDLOG_DEBUG("handle a set key:{}", sendValue);
	//#endif
	//	std::string htmlContent = "{";
	//	auto createRet = (req->Path() == "/create_onePGM") ? m_ledSever->create_onPGM_byCode(sendValue, extSetting)
	//													   : m_ledSever->createPGM_withLspj(sendValue, extSetting);
	//
	//	htmlContent += fmt::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));
	//	htmlContent += fmt::format("\"sendValue\":\"{}\",", (sendValue));
	//	htmlContent += m_ledSever->getNetWorkIDList();
	//	htmlContent += "}";
	//	/**/
	//	SPDLOG_DEBUG(htmlContent);
	//	return res->String(htmlContent);
	//	return res->Json(htmlContent);
}

inline int HV_serverImp::http_server(int httpPort)
{
	// HTTPS
	httplib::Server router;

	router.Get("/hi", [](const httplib::Request&, httplib::Response& res)
		{ res.set_content("Hello World!", "text/plain"); });

	//router.Static("/", "./webPage");

	//router.Post("/leds/add", [&](const httplib::Request &req, httplib::Response &resp)
	//			{
	//	std::string htmlContent;
	//		htmlContent = "{";
	//		htmlContent += true ? "sucess" : "failed";
	//		htmlContent += "}";
	//		return resp->Json(htmlContent); });
	//router.GET("/leds/list", [this](const httplib::Request &req, httplib::Response &resp)
	//		   { return list_Handler(req, resp); });

	router.Get("/list", [this](const httplib::Request& req, httplib::Response& resp)
		{ return list_Handler(req, resp); });

	/*router.Get("/paths", [this](const httplib::Request &req, httplib::Response &resp)
			   { return resp->Json(router.Paths()); });*/




	router.Get("/test", [this](const httplib::Request& req, httplib::Response& resp) {
		m_ledSever->test();
		resp.set_content("done", "text/plain");
		});
	;

	//router.GET("/create_onePGM", [this](const httplib::Request &req, httplib::Response &resp)
	//		   { return CreatePGM_Handler(req, resp); });

	//router.GET("/create_withPGM", [this](const httplib::Request &req, httplib::Response &resp)
	//		   { return CreatePGM_Handler(req, resp); });
	//router.GET("/updateLedContent_LanfengLED", [this](const httplib::Request &req, httplib::Response &resp)
	//		   { return updateLedContent_LanfengLED(req, resp); });
	//router.POST("/updateLedContent_LanfengLED", [this](const httplib::Request &req, httplib::Response &resp)
	//			{ return updateLedContent_LanfengLED(req, resp); });
	auto updatecontentHandler = [this](const httplib::Request& req, httplib::Response& resp)
		{
			//�˴���json
			std::unique_lock<std::mutex> lock(led_neima_mutex, std::defer_lock);
			if (!lock.try_lock())
			{
				return resp.set_content("led_neima_mutex locked", "");
			}
			std::string sendValue = req.body;
			nlohmann::json jsonValue = nlohmann::json::parse(sendValue);
			//SPDLOG_DEBUG(sendValue);
			LedTask task;
			from_json(jsonValue, task);

			auto createRet = m_ledSever->updateLedContent_JSON(task);

			auto htmlContent = fmt::format("{{\"ret\":{},\"msg\":\" {} \" }}", std::get<0>(createRet), std::get<1>(createRet));

			resp.set_content(htmlContent, "application/json");
		};
	router.Get("/updatecontent", updatecontentHandler);
	router.Post("/updatecontent", updatecontentHandler);

	router.Get("/empty_plot", [&](const httplib::Request& req, httplib::Response& resp)
		{
			// url = f'/empty_plot?ledids={ledids}&empty_plot={empty_plot}&pgmfilepath={pgmfilepath}&park_id={park_id}&fontcolor={fontcolor}'
			std::unique_lock<std::mutex> lock(led_neima_mutex, std::defer_lock);
			if (!lock.try_lock())
			{
				return resp.set_content("led_neima_mutex locked", "");
			}
			std::string ledids = "";
			if (req.has_param("ledids")) {
				ledids = req.get_param_value("ledids");
			}

			std::string  empty_plotcount = "";
			if (req.has_param("empty_plotcount"))
				empty_plotcount = req.get_param_value("empty_plotcount");
			std::vector<std::string> empty_plot;
			for (int i = 0; i < atoi(empty_plotcount.data()); i++)
			{
				empty_plot.push_back(req.get_param_value(fmt::format("empty_plot_{}", i)));
			}
			std::string  pgmfilepath = "";
			if (req.has_param("pgmfilepath"))
				pgmfilepath = req.get_param_value("pgmfilepath");
			ExtSeting extSetting;

			std::string  fontSize = "";
			if (req.has_param("fontsize"))
				fontSize = req.get_param_value("fontsize");

			if (fontSize != "")
				extSetting.FontSize = atoi(fontSize.data());

			std::string fontColor = "";
			if (req.has_param("fontsize"))
				fontColor = req.get_param_value("fontcolor");

			if (fontColor != "-1")
				extSetting.FontColor = atoi(fontColor.data());

			extSetting.backGroundImage = "";

			if (req.has_param("backgroundImage"))
				extSetting.backGroundImage = req.get_param_value("backgroundImage");


			std::string htmlContent = "{";
			auto createRet = m_ledSever->createPGM_empty_plot(ledids, empty_plot, pgmfilepath, extSetting);

			htmlContent += fmt::format("\"ret\":{},\"msg\":{{ {} }},", std::get<0>(createRet), std::get<1>(createRet));
			htmlContent += fmt::format("\"ledids\":\"{}\",", (ledids));
			//htmlContent += fmt::format("\"empty_plot\":\"{}\",", (empty_plot));
			htmlContent += fmt::format("\"pgmfilepath\":\"{}\",", (pgmfilepath));
			htmlContent += fmt::format("\"backgroundImage\":\"{}\",", (extSetting.backGroundImage));
			htmlContent += m_ledSever->getNetWorkIDList();
			htmlContent += "}";
			/**/
		   // SPDLOG_DEBUG(htmlContent);
			resp.set_content(htmlContent, "text/plain");
		});

	//router.GET("/reloadpgm", [this](const httplib::Request &req, httplib::Response &res)
	//		   {		
	//		auto 	sendValue = req->GetParam("key");
	//		std::string htmlContent;
	//		htmlContent = "{";
	//		htmlContent += (IConfig.ReloadPGM(sendValue) ? "sucess" : "failed");
	//		htmlContent += "}";
	//		return res->Json(htmlContent); });

	//router.GET("/neima", [this](const httplib::Request &req, httplib::Response &res)
	//		   {
	//	if (auto key = req->GetParam("key"); key != hv::empty_string)
	//	{
	//		std::lock_guard<std::mutex> lock(led_neima_mutex);
	//		auto v = split_string(key);
	//		int nlen = makeNeiMa(neima, v);
	//		int a = 0;
	//		int count = m_tcpSrv.broadcast(neima, nlen);
	//		std::string htmlJson;
	//		htmlJson =fmt::format("{{\"ret\":{} ,\"ledCount\":{} ,\"msg\":\"{}\"}}",a,count,"ok");
	//		return res->Json(htmlJson);
	//	}
	//	return res->Json(fmt::format("{{\"ret\":-1 ,\"ledCount\":0 ,\"msg\":\"miss key\"}}")); });

	printf("http:%d\n", httpPort);

	router.listen("0.0.0.0", httpPort);
	return 0;
}