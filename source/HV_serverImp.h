#pragma once
#include "cinatra.hpp"
using namespace cinatra;


using HttpRequest = request;
using HttpResponse = response;
class LED_Server;
#include "socket/tcpServer.h"
class HV_serverImp {
public:
	int start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer);
private:
	http_server * httpServer;
	int list_Handler(HttpRequest& req, HttpResponse& resp);;
	int CreatePGM_Handler(HttpRequest& req, HttpResponse& res);;

	int start_http_server(int httpPort);

	int tcp_server(int _proxy_port, int _backend_port);
public:

	LED_Server* m_ledSever;

	int backend_port = 0;
	ASIOTcpServer::TcpServer tcpServer;

	std::mutex led_neima_mutex;
	char neima[256];
	
};