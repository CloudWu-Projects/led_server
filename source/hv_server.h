#pragma once
class HV_serverImp;
class LED_Server;
class HV_server {
public:
	int start(int httpPort, int ledPort, LED_Server* ledServer);

private:
	HV_serverImp* Imp;
};