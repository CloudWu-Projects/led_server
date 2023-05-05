#pragma once
#include <string>
#include <tuple>
#include "config.h"
class CLedDll;
typedef void*				HPROGRAM;
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createAProgram(std::wstring& showText);
	std::tuple<int, std::string> createAProgram2(std::wstring& showText);
private:
	CLedDll *g_Dll;
	std::tuple<int, std::string> sendProgram(std::wstring WnetworkID, HPROGRAM hProgram);
	std::tuple<int, std::string> createAProgram(std::wstring networkID, std::wstring& showText, const Config::LEDParam& ledParam);

	HPROGRAM createAProgram_withLspj( std::wstring& showText);

};
