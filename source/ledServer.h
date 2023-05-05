#pragma once
#include <string>
#include <tuple>
#include "config.h"
class CLedDll;
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createAProgram(std::wstring& showText, const Config::LEDParam &ledParam);

private:
	CLedDll *g_Dll;
	std::tuple<int, std::string> createAProgram(std::wstring networkID, std::wstring& showText, const Config::LEDParam& ledParam);


};
