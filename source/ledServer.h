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
	std::tuple<int,std::string> createAProgram(std::wstring& showText);
	std::tuple<int, std::string> createAProgram2(std::wstring& showText);
private:
	CLedDll *g_Dll;
	std::tuple<int, std::string> sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram);
	std::tuple<int, std::string> createAProgram(NETWORKID networkID, std::wstring& showText, const Config::LEDParam& ledParam);

	HPROGRAM createAProgram_withLspj( std::wstring& showText);

};
