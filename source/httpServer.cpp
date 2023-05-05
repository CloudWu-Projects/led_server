#include "httpServer.h"

#include "httpServerWrapper.h"
#include "ledServer.h"
#include "stringHelper.h"
#include "config.h"

void HttpServer::startHttpServer(const Config* config, LED_Server& ledServer)
{
	auto SetHandler =
		[&ledServer, &config](const httplib::Request& req, httplib::Response& res)
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
			if (req.has_param("key"))
			{
				sendValue = to_wide_string(req.get_param_value("key"));
			}
		}
		SPDLOG_DEBUG(L"handle a set key:{}", sendValue);

		auto createRet = ledServer.createAProgram(sendValue, config->ledParam);
		std::string htmlContent = "{";
		htmlContent += fmt::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));
		htmlContent += fmt::format("\"sendValue\":\"{}\",", to_byte_string(sendValue));
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
				res.set_content(htmlContent, "application/json"); })
		.Get("/set", SetHandler)
					.Post("/set", SetHandler);

				svr.listen("0.0.0.0", config->httpPort);
}