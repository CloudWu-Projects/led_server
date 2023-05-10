#pragma once

#include "ledServer.h"


class HttpHandler {
public:
	HttpHandler(LedSever* _ledServer) :ledServer(_ledServer)
	{
	}
	int listLed(const httplib::Request& req, httplib::Response& res)
	{
		std::string htmlContent;
		htmlContent = "{" + ledServer->getNetWorkIDList() + "}";
		res.set_content(htmlContent, "application/json");
	}

private:
	LedSever* ledServer;
};