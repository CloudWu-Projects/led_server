#pragma once
#include <string>
#include <tuple>
#include "config.h"
#include <map>
#include "json.hpp"

#include "ledAPI.h"
#include "structDefine.h"

#ifdef UNICODE
#define NETWORKID std::wstring
#else
#define NETWORKID std::string
#endif
struct ExtSeting {
	int FontSize = -1;
	int FontColor = -1;//BBGGRR（如：红色 0xff 绿色 0xff00 黄色 0xffff）
	std::string backGroundImage="";
};

struct LedContent {};
class LED_Server:public LedAPI
{
public:
	LED_Server();
	int start(int port );
	std::string getNetWorkIDList();
	std::tuple<int, std::string> updateLedContent_JSON(const LedTask& ledTask);
	std::tuple<int,std::string> createPGM_empty_plot(const std::string& ledids,const std::vector<std::string> &empty_plot,const std::string& pgmfilepath,ExtSeting&extSetting);
	void test();
private:


	HPROGRAM createAProgram_NoLSPJ(const  std::vector<std::string>& showText,const LED& led,ExtSeting *m_extSetting);

	
	int api_createSingleLineArea(HPROGRAM m_hProgram, Area& area, const std::string& pShowText, ExtSeting* m_extSetting);
	int api_createTimeClockArea(HPROGRAM m_hProgram, Area& area,ExtSeting *m_extSetting);
	int api_createNeimaArea(HPROGRAM m_hProgram, Area& area,const char* pShowText,ExtSeting *m_extSetting);

	int m_nProgramNo=0;

	
};
