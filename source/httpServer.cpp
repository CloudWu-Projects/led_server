#include "httpServer.h"

#include "httpServerWrapper.h"
#include "ledServer.h"
#include "stringHelper.h"
#include "config.h"

void HttpServer::startHttpServer(LED_Server& ledServer)
{
	auto SetHandler =
		[&ledServer](const httplib::Request& req, httplib::Response& res)
	{
		res.set_header("Access-Control-Allow-Origin", "*");
		ExtSeting extSetting;
		std::string sendValue = "test NUll";
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
		}

		if (req.has_param("content"))
		{
			sendValue = to_wide_string(req.get_param_value("content"));
		}
		if (req.has_param("fontsize"))
		{
			extSetting.FontSize = atoi(req.get_param_value("fontsize").data());
		}
#ifdef UNICODE
		SPDLOG_DEBUG(L"handle a set key:{}", sendValue);
#else
		SPDLOG_DEBUG("handle a set key:{}", sendValue);
#endif
		std::string htmlContent = "{";
		auto createRet = (req.path == "/create_onePGM") ? 
				ledServer.create_onPGM_byCode(sendValue, extSetting)
				: ledServer.createPGM_withLspj(sendValue, extSetting);
		
		htmlContent += fmt::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));
		htmlContent += fmt::format("\"sendValue\":\"{}\",", to_byte_string(sendValue));
		htmlContent += ledServer.getNetWorkIDList();
		htmlContent += "}";
		/**/
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
		.Post("/set", SetHandler)
		.Get("/create_onePGM", SetHandler)
		.Get("/create_withPGM", SetHandler)
		.Get("/reloadpgm", [&ledServer](const httplib::Request& req, httplib::Response& res)
			{
				std::string htmlContent;
				htmlContent = "{";
				htmlContent += (IConfig.ReloadPGM() ? "sucess" : "failed");
				htmlContent += "}";
				res.set_content(htmlContent, "application/json");
			})
		.Get("/b", [&ledServer](const httplib::Request& req, httplib::Response& res)
			{
						
				//res.set_content(htmlContent, "application/json");
			})
		.listen("0.0.0.0", IConfig.httpPort);
}