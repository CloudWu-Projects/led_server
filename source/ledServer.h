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
	std::tuple<int,std::string> createPGM_withLspj(const std::string& ledids,const std::string& empty_plot,const std::string& pgmfilepath,ExtSeting&extSetting);
	std::tuple<int, std::string> create_onPGM_byCode(std::string& showText, ExtSeting& extSetting);
	void test();
private:

#ifdef WIN32
	CLedDll *g_Dll;
#endif
	std::tuple<int, std::string> sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram);
	std::tuple<int, std::string> createAProgram(NETWORKID networkID, std::string& showText, const Config::LEDParam& ledParam,ExtSeting *m_extSetting);

	HPROGRAM createAProgram_withLspj(const  std::string& showText,std::vector<LED> &leds,ExtSeting *m_extSetting);

	int createSingleLineArea(Area& area,const char* pShowText,ExtSeting *m_extSetting);
	int createTimeClockArea(Area& area,ExtSeting *m_extSetting);
	int createNeimaArea(Area& area,const char* pShowText,ExtSeting *m_extSetting);

	HPROGRAM m_hProgram=nullptr;
	int m_nProgramNo=0;

	
};
