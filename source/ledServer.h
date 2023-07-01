#pragma once
#include <string>
#include <tuple>
#include "config.h"
#include <map>
class CLedDll;
typedef void*				HPROGRAM;
#ifdef UNICODE
#define NETWORKID std::wstring
#else
#define NETWORKID std::string
#endif
struct ExtSeting {
	int FontSize = -1;
	int FontColor = -1;//BBGGRR（如：红色 0xff 绿色 0xff00 黄色 0xffff）
};
struct LedContent {
	std::vector<std::string> ledids;
	bool isIP;
	std::string park_id;
	 std::string pgmfilepath;
	std::map<std::string, std::string> ledid2content;
};
class LED_Server
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int,std::string> createPGM_withLspj(std::string& showText, ExtSeting&extSetting);
	std::tuple<int,std::string> createPGM_withLspj(LedContent& ledContent, ExtSeting&extSetting);
	std::tuple<int,std::string> createPGM_withLspj(const std::string& ledids,const std::string& empty_plot,const std::string& pgmfilepath,ExtSeting&extSetting);
	std::tuple<int, std::string> create_onPGM_byCode(std::string& showText, ExtSeting& extSetting);
	void test();
private:

#ifdef WIN32
	CLedDll *g_Dll;
#endif
	std::tuple<int, std::string> sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram, bool isIP=false);
	std::tuple<int, std::string> createAProgram(NETWORKID networkID, std::string& showText, const Config::LEDParam& ledParam,ExtSeting *m_extSetting);

	HPROGRAM createAProgram_withLspj(const  std::string& showText,std::vector<LED> &leds,ExtSeting *m_extSetting);

	HPROGRAM createAProgram_withLspj(LedContent& ledContent, std::vector<LED>& leds, ExtSeting* m_extSetting);
	int createSingleLineArea(Area& area, const char* pShowText, ExtSeting* m_extSetting);
	int createTimeClockArea(Area& area,ExtSeting *m_extSetting);
	int createNeimaArea(Area& area,const char* pShowText,ExtSeting *m_extSetting);

	HPROGRAM m_hProgram=nullptr;
	int m_nProgramNo=0;

	
};
