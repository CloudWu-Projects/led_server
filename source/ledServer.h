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
struct ExtSeting {
	int FontSize = 14;
};
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createPGM_withLspj(std::string& showText, ExtSeting&extSetting);
	std::tuple<int, std::string> create_onPGM_byCode(std::string& showText, ExtSeting& extSetting);
private:

#ifdef WIN32
	CLedDll *g_Dll;
#endif
	std::tuple<int, std::string> sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram);
	std::tuple<int, std::string> createAProgram(NETWORKID networkID, std::string& showText, ExtSeting& extSetting, const Config::LEDParam& ledParam);

	HPROGRAM createAProgram_withLspj( std::string& showText, ExtSeting& extSetting);

};
