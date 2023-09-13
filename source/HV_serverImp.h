#pragma once


#include "httplib.h"
class LED_Server;


class HV_serverImp {
public:
	int start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer);
private:
	int list_Handler(const httplib::Request& req, httplib::Response& resp);
	httplib::Server httpServer;
	int CreatePGM_Handler(const httplib::Request& req, httplib::Response& res);;

	int http_server(int httpPort);

public:

	LED_Server* m_ledSever;

	int backend_port = 0;

	std::mutex led_neima_mutex;
	char neima[256];
	
};