#pragma once
#include "hv/HttpServer.h"
#include "hv/TcpServer.h"

#include "hv/hloop.h"
#include "hv/hsocket.h"

using namespace hv;


class LED_Server;
class HV_serverImp {
public:
	int start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server* ledServer);
private:
	HttpServer httpServer;
	int list_Handler(HttpRequest* req, HttpResponse* resp);;
	int CreatePGM_Handler(HttpRequest* req, HttpResponse* res);;
	int updateLedContent_LanfengLED(HttpRequest* req, HttpResponse* resp);

	int http_server(int httpPort);

	int tcp_server(int _proxy_port, int _backend_port);
public:

	LED_Server* m_ledSever;

	HttpService router;
	int backend_port = 0;
	TcpServer m_tcpSrv;

	std::mutex led_neima_mutex;
	char neima[256];
	
};