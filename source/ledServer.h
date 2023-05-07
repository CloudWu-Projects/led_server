#pragma once
#include <string>
#include <tuple>
#include "config.h"
class CLedDll;
typedef void*				HPROGRAM;
#ifdef UNICODE
#define NETWORKID std::wstring
#else
#define NETWORKID std::string
#endif
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createAProgram(std::string& showText);
	std::tuple<int, std::string> createAProgram2(std::string& showText);
private:

#ifdef WIN32
	CLedDll *g_Dll;
#endif
	std::tuple<int, std::string> sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram);
	std::tuple<int, std::string> createAProgram(NETWORKID networkID, std::string& showText, const Config::LEDParam& ledParam);

	HPROGRAM createAProgram_withLspj( std::string& showText);

};
