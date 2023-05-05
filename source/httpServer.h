#pragma once
class LED_Server;
class Config;
class HttpServer
{
public:
     void startHttpServer(const Config* g_Config, LED_Server &ledServer);
   
};