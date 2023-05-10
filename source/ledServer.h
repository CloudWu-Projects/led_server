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
typedef struct  AREARECT;
struct ExtSeting {
	int FontSize = 14;
};
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createPGM_withLspj(bool isJson,std::string& showText, ExtSeting&extSetting);
	std::tuple<int, std::string> create_onPGM_byCode(std::string& showText, ExtSeting& extSetting);
private:

#ifdef WIN32
	CLedDll *g_Dll;
#endif
	std::tuple<int, std::string> sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram);
	std::tuple<int, std::string> createAProgram(NETWORKID networkID, std::string& showText, const Config::LEDParam& ledParam);

	HPROGRAM createAProgram_withJson(std::string& showText);
	HPROGRAM createAProgram_withLspj( std::string& showText);

	int createSingleLineArea(Area& area, char* pShowText);
	int createTimeClockArea(Area& area);
	int createNeimaArea(Area& area, char* pShowText);

	HPROGRAM m_hProgram=nullptr;
	int m_nProgramNo=0;
	ExtSeting *m_extSetting;
};
