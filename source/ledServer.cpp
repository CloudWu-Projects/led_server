#include "ledServer.h"
#include "ledAPI.h"

#define TRACE printf

#include <string>
#include <set>
#include <thread>
#include <mutex>
//#include <format>
#include "json.hpp"

#include "stringHelper.h"
#include "logLib.h"
std::mutex queue_mutex;
#ifdef UNICODE
	std::set<std::wstring> clientNetWorkID;
#else
	std::set<std::string> clientNetWorkID;
#endif


int LedServerCallback(int Msg, int wParam, void* lParam)
{
	SPDLOG_DEBUG("LedServerCallback msg:{} ", Msg);
	switch (Msg)
	{
	case LV_MSG_CARD_ONLINE:
	{
		LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;

#ifdef UNICODE
		SPDLOG_DEBUG(L"LV_MSG_CARD_ONLINE port:{} ip:{} networkId:{}\n", pCardInfo->port, pCardInfo->ipStr, pCardInfo->networkIdStr);
#else
		SPDLOG_DEBUG("LV_MSG_CARD_ONLINE port:{} ip:{} networkId:{}\n", pCardInfo->port, pCardInfo->ipStr, pCardInfo->networkIdStr);
#endif

		{
			std::lock_guard<std::mutex> lock(queue_mutex);
			clientNetWorkID.emplace(pCardInfo->networkIdStr);
		}

	}
	break;
	case LV_MSG_CARD_OFFLINE:
	{
		LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;

#ifdef UNICODE
		SPDLOG_DEBUG(L"LV_MSG_CARD_OFFLINE port:{} ip:{} networkId:{}\n", pCardInfo->port, pCardInfo->ipStr, pCardInfo->networkIdStr);
#else
		SPDLOG_DEBUG("LV_MSG_CARD_OFFLINE port:{} ip:{} networkId:{}\n", pCardInfo->port, pCardInfo->ipStr, pCardInfo->networkIdStr);
#endif
		{
			std::lock_guard<std::mutex> lock(queue_mutex);
			clientNetWorkID.erase(pCardInfo->networkIdStr);
		}
	}
	break;
	}
	return 0;
}

LED_Server::LED_Server()
{

}

int LED_Server::start(int port)
{
#ifdef WIN32
	g_Dll = new CLedDll();
	if (!g_Dll->InitDll())
		return -1;

	g_Dll->LV_RegisterLedServerCallback((SERVERINFOCALLBACK)LedServerCallback);
	int ret = g_Dll->LV_LedInitServer(port);
#else

	LV_RegisterServerCallback((SERVERINFOCALLBACK)LedServerCallback);
	int ret = LV_InitServer(port);
#endif
	printf("LV_LedInitServer ret=%d\n", ret);
	return ret;
}

std::string LED_Server::getNetWorkIDList()
{
	std::string htmlContent = "\"idlist\":[";
	{
		std::lock_guard<std::mutex> lock(queue_mutex);

		int i = 0;
		for (auto id : clientNetWorkID)
		{
			if (i != 0)
				htmlContent += ",";
			i++;
			htmlContent += "\"" + 
#ifdef WIN32
				to_byte_string(id) 
#else
				id
#endif
				+ "\"";
		}
	}
	htmlContent += "]";
	return htmlContent;
}

std::tuple<int, std::string> LED_Server::updateLedContent_JSON(const LedTask& ledTask)
{
	std::lock_guard<std::mutex> lock(queue_mutex);
	if (clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty");
	auto ledid = ledTask.LED_id;

	if (!clientNetWorkID.contains(ledid))
	{
		return std::make_tuple(-1, fmt::format("networkID canot found [{}]",ledid));
	}

	HPROGRAM m_hProgram = nullptr;

	int ledType =      ledTask.led_info.LedType;
	int ledWidth =     ledTask.led_info.LedWidth;
	int ledHeight =    ledTask.led_info.LedHeight;
	int ledColor =     ledTask.led_info.LedColor;
	int ledGraylevel = ledTask.led_info.LedGray;

	std::string backGroundImage = ledTask.background;

	SPDLOG_DEBUG("ledid:{} ledType:{} ledWidth:{} ledHeight:{} ledColor:{} ledGraylevel:{} \nbackground:{}", ledid, ledType, ledWidth, ledHeight, ledColor, ledGraylevel, backGroundImage);
	m_hProgram = api_createProgram( ledWidth, ledHeight, ledColor, ledGraylevel);
	if (m_hProgram == nullptr)
	{
		return std::make_tuple(-1, "createAProgram failed.");
	}
	int nResult = 0;
	do {
		int areaId = 1;
		if (backGroundImage.length() > 0)
		{
			nResult =api_addBackground(m_hProgram, ledWidth, ledHeight, backGroundImage.c_str());
			if (nResult)break;
			areaId++;
		}
		
		for (int i=0; i<ledTask.data.size(); i++) {			
			Area area{
				.AreaNo = areaId,
				.AreaRect = {
					.left=ledTask.data[i].left,
					.top = ledTask.data[i].top,
					.width = ledTask.data[i].width,
					.height = ledTask.data[i].height
					},
			};
			if (area.AreaRect.left == -1 || area.AreaRect.top == -1 || area.AreaRect.width == -1 || area.AreaRect.height == -1)
			{
				area.AreaRect.left = 1;
				area.AreaRect.top = ledHeight * i / ledTask.data.size()+1;
				area.AreaRect.width = ledWidth - 2;
				area.AreaRect.height = ledHeight/ledTask.data.size() -1;
				if (area.AreaRect.height + area.AreaRect.top >= ledHeight)
					area.AreaRect.height = ledHeight - area.AreaRect.top - 1;
			}

			area.singleLineArea.DelayTime=3;
			area.singleLineArea.InStyle=0;
			area.singleLineArea.OutStyle=0;
			area.singleLineArea.InSpeed=32;

			areaId++;
			//SPDLOG_DEBUG("id:{} left:{} top:{} width:{} height:{} ", area.AreaNo, area.AreaRect.left, area.AreaRect.top, area.AreaRect.width, area.AreaRect.height);

			ExtSeting extSetting;
			extSetting.FontColor = ledTask.data[i].F_color;
			area.singleLineArea.FontSize = ledTask.data[i].F_size;
			auto showText = ledTask.data[i].F_message;			
			nResult = api_createSingleLineArea(m_hProgram,area,showText.data(),&extSetting);
			if (nResult)
				break;			
		}
		if (nResult)
			break;
		nResult = api_send(m_hProgram, ledType, ledid);
	} while (0);
	api_deleteProgram(m_hProgram); m_hProgram = nullptr;
	if (nResult)
	{
		TCHAR ErrStr[256];
		api_getLastError(nResult, ErrStr);
		std::string msg = ErrStr;
		if (55992 == nResult)
		{
			nResult=api_setBasicInfo(ledid, ledType, ledWidth, ledHeight, ledColor, ledGraylevel);
			msg = "屏幕参数不正确 自动修正";
			if (nResult)
			{
				api_getLastError(nResult, ErrStr);
				msg = fmt::format( "屏幕参数不正确 自动修正失败 {}",ErrStr);
			}
		}
		return std::make_tuple(nResult, msg);
	}

	return std::make_tuple(0, "sucessd");
}

std::tuple<int, std::string> LED_Server::createPGM_empty_plot(const std::string &ledids, const std::vector<std::string> &empty_plot, const std::string &pgmfilepath, ExtSeting &extSetting)
{
	std::lock_guard<std::mutex> lock(queue_mutex);
	if (clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty");
	std::vector<LED> leds;

	LED_lsprj led_lsprj;
	if(!led_lsprj.loadFile(pgmfilepath.data(),leds))
	{
		return std::make_tuple(-2, fmt::format("load file[{}] failed.",pgmfilepath));
	}
	std::string retHtml = "";
	
	auto vecLedids = split_string(ledids, ',');
	std::string noinItem="";
	std::string sucessItem="";
	HPROGRAM m_hProgram = nullptr;

	for (auto ledid : vecLedids)
	{
		if (!clientNetWorkID.contains(ledid))
		{
			if(noinItem.length()>0)
				noinItem+=",";
			noinItem += "\"" + ledid + "\"";
			continue;
		}
		int ledType = leds[0].LedType;
		if (m_hProgram == nullptr)
		{	
			int ledWidth = leds[0].LedWidth;
			int ledHeight = leds[0].LedHeight;
			int ledColor = leds[0].LedColor;
			int ledGraylevel = leds[0].LedGray;
			SPDLOG_DEBUG("ledid:{} ledType:{} ledWidth:{} ledHeight:{} ledColor:{} ledGraylevel:{} \nbackground:{}", ledid, ledType, ledWidth, ledHeight, ledColor, ledGraylevel,extSetting.backGroundImage);
			//m_hProgram = createAProgram_withLspj(empty_plot, leds,&extSetting);
			m_hProgram = createAProgram_NoLSPJ(empty_plot, leds[0], &extSetting);
		}
		if (m_hProgram == nullptr)
		{
			retHtml = "createAProgram_withLspj failed.";
			break;
		}
		int r = api_send(m_hProgram, ledType, ledid);
		
		TCHAR error[256]; 
		if(r)
			api_getLastError(r, error);
		sucessItem += "\"" + fmt::format("api_send [{}] {} {},",ledid, r ? "failed." : "sucess.",error);
		sucessItem += "\"";
		
	}
	std::string noInHtml="\"noin\":["+noinItem+"]";
	
	std::string sucessHtml="\"sucess\":["+sucessItem+"]";
	
	retHtml = noInHtml+","+sucessHtml;
	api_deleteProgram(m_hProgram);
	
	m_hProgram = nullptr;

	return std::make_tuple(0, retHtml);
}


HPROGRAM LED_Server::createAProgram_NoLSPJ(const std::vector<std::string>& showText, const LED& led, ExtSeting* m_extSetting)
{
	int nResult = 0;
	HPROGRAM hProgram = nullptr;	
	if (led.programs.empty())
	{
		SPDLOG_ERROR("createAProgram_NoLSPJ failed. programs.empty()");
		return nullptr;
	}
	auto p = led.programs[0];
	std::string callAPi = "";
	do {
		hProgram = api_createProgram(led.LedWidth, led.LedHeight, led.LedColor, led.LedGray);
		if (hProgram == nullptr)break;

		if (m_extSetting->backGroundImage != "")
		{
			nResult = api_addBackground(hProgram, led.LedWidth, led.LedHeight, m_extSetting->backGroundImage);
			if (nResult)
			{
				callAPi = "api_addBackground";
				break;
			}
		}
		int iCount = 0;
		//先计算位置
		Area areas[4];

		static AREARECT g_arect[4] = { 
			{1,1,78,39},
			{1,41,78,39}, 
			{1,81,78,39}, 
			{1,121,78,38}};
		for (auto area : p.areas)
		{
			area.AreaNo += 1;
			area.AreaRect = g_arect[iCount];

			const char* pShowText = (const char*)showText[iCount].data();

			if (p.areas.size() > 1 && showText.size() == p.areas.size())
				pShowText = showText[iCount].data();

			if (area.areaType == Area::SINGLELINEAREA)
				nResult = api_createSingleLineArea(hProgram,area, pShowText, m_extSetting);
			else if (area.areaType == Area::TIME_AREA)
				nResult = api_createTimeClockArea(hProgram, area, m_extSetting);
			else if (area.areaType == Area::NEIMA_AREA)
				nResult = api_createNeimaArea(hProgram, area, pShowText, m_extSetting);

			iCount++;
			if (nResult) {
				callAPi = "createSingleLineArea";
				break;
			}
		}
	} while (0);

	if (nResult)
	{
		TCHAR ErrStr[256];
		api_getLastError(nResult,ErrStr);
		SPDLOG_ERROR("createAProgram_withLspj error ,{} @api {}", ErrStr, callAPi);
		api_deleteProgram(hProgram); hProgram = nullptr;
	}
	return hProgram;
}

int LED_Server::api_createSingleLineArea(HPROGRAM m_hProgram,Area&area,const std::string &pShowText,ExtSeting *m_extSetting)
{
	int nResult = 0;
#ifdef WIN32
	nResult = g_Dll->LV_AddImageTextArea(m_hProgram, m_nProgramNo, area.AreaNo, &area.AreaRect, 1);
#else
	nResult = LV_AddImageTextArea(m_hProgram, 0, area.AreaNo, &area.AreaRect, 1);
#endif
	if (nResult)return nResult;

	PLAYPROP PlayProp;//显示属性
	PlayProp.DelayTime = area.singleLineArea.DelayTime;
	PlayProp.InStyle = area.singleLineArea.InStyle;
	PlayProp.OutStyle = area.singleLineArea.OutStyle;
	PlayProp.Speed = area.singleLineArea.InSpeed;

	FONTPROP FontProp; // 文字属性
	memset(&FontProp, 0, sizeof(FONTPROP));
#ifdef UNICODE
	_tcscpy(FontProp.FontName, L"宋体");
#else 
#ifdef WIN32
	strcpy(FontProp.FontName, "宋体");
#else
	strcpy(FontProp.FontPath, "./simsun.ttc");
#endif

#endif

	FontProp.FontSize = area.singleLineArea.FontSize;
	if(m_extSetting->FontSize!=-1)
		FontProp.FontSize = m_extSetting->FontSize;
		
	FontProp.FontColor = area.singleLineArea.FontColor;
	if(m_extSetting->FontColor!=-1)
		FontProp.FontColor = m_extSetting->FontColor;
	
	//BBGGRR（如：红色 0xff 绿色 0xff00 黄色 0xffff）
#ifdef WIN32
	nResult = g_Dll->LV_AddSingleLineTextToImageTextArea(m_hProgram, m_nProgramNo, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, &PlayProp);
#else
	int nAlignment = 0;
	PlayProp.InStyle = 2;
	PlayProp.DelayTime = 1;
	PlayProp.Speed = 1;
	if (pShowText.length() < 36)
	{
		PlayProp.InStyle = 0;
		nAlignment = 2;
	}
	nResult = LV_AddSingleLineTextToImageTextArea(m_hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText.data(), &FontProp, &PlayProp);
	//else
	//	nResult = LV_AddMultiLineTextToImageTextArea(m_hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, &PlayProp, nAlignment, FALSE);//通过字符串添加一个多行文本到图文区，参数说明见声明注示
		
#endif
	//nResult = g_Dll->LV_AddStaticTextToImageTextArea(hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, area.DelayTime, 0, true);
	
	return nResult;
}

int LED_Server::api_createTimeClockArea(HPROGRAM m_hProgram, Area& area,ExtSeting *m_extSetting)
{
	//添加一个数字时钟区域 

	

#ifdef WIN32
	strcpy(area.clockIfo.ShowStrFont.FontName, "宋体");
#else
	strcpy(area.clockIfo.ShowStrFont.FontPath, "./simsun.ttc");
#endif

	area.clockIfo.ShowStrFont.FontSize = m_extSetting->FontSize;
	area.clockIfo.ShowStrFont.FontColor = COLOR_RED;
	int nResult = 0;
#ifdef WIN32
	nResult = g_Dll->LV_AddDigitalClockArea(m_hProgram, m_nProgramNo, area.AreaNo, &area.AreaRect, &area.clockIfo);
#else
	nResult = LV_AddDigitalClockArea(m_hProgram, m_nProgramNo, area.AreaNo, &area.AreaRect, &area.clockIfo);
#endif
	return nResult;
}

int LED_Server::api_createNeimaArea(HPROGRAM m_hProgram, Area& area,const char* pShowText,ExtSeting *m_extSetting)
{
	PLAYPROP PlayProp;//显示属性
	PlayProp.DelayTime = area.neiMaArea.DelayTime;
	PlayProp.InStyle = area.neiMaArea.InStyle;
	PlayProp.OutStyle = area.neiMaArea.OutStyle;
	PlayProp.Speed = area.neiMaArea.PlaySpeed;
	int fontSize = area.neiMaArea.FontSize;
	if (fontSize == 0)
		fontSize = m_extSetting->FontSize;
#ifdef WIN32
	return g_Dll->LV_AddNeiMaArea(m_hProgram, m_nProgramNo, area.AreaNo, &area.AreaRect, pShowText, fontSize, area.neiMaArea.FontColor, &PlayProp);
#else
	return LV_AddNeiMaArea(m_hProgram, m_nProgramNo, area.AreaNo, &area.AreaRect, pShowText, fontSize, area.neiMaArea.FontColor, &PlayProp);
#endif
}


#ifndef WIN32

//一个节目下有两个多行文本区域,并且有背景
void onTwoMutiTextAndBackground2(const char* pIp, int ledType, int ledWidth, int ledHeight, int colorType, int grayLevel)
{
	printf("一个节目下有两个多行文本区域,并且有背景\n");
	int nResult;
	COMMUNICATIONINFO CommunicationInfo;
	memset(&CommunicationInfo, 0, sizeof(COMMUNICATIONINFO));
	CommunicationInfo.LEDType = ledType;
	//TCP通讯********************************************************************************
	CommunicationInfo.SendType = 0;//设为固定IP通讯模式，即TCP通讯
	_tcscpy(CommunicationInfo.IpStr, pIp);//给IpStr赋值LED控制卡的IP
	CommunicationInfo.LedNumber = 1;
	//广播通讯********************************************************************************
	//CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
	//广域网通讯
	CommunicationInfo.SendType = 4;//设为广域网通讯模式
	_tcscpy(CommunicationInfo.NetworkIdStr, pIp);//指定唯一网络ID

	/***第一步创建节目句柄***开始***/
	HPROGRAM hProgram;//节目句柄

	ledWidth = 80;
	ledHeight = 40;
	colorType = 3;
	grayLevel = 2;
	hProgram = LV_CreateProgramEx(ledWidth, ledHeight, colorType, grayLevel, 0);//根据传的参数创建节目句柄，注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误

	/***第一步创建节目句柄***结束***/

	/***第二步添加一个节目***开始***/
	int nProgramNo = 0;
	nResult = LV_AddProgram(hProgram, nProgramNo, 0, 1);
	if (nResult)
	{
		char ErrStr[256];
		LV_GetError(nResult, 256, ErrStr);
		printf("%s\n", ErrStr);
		return;
	}
	/***第二步添加一个节目***结束***/
	int nAreaId = 1;
	/***第三步添加一个图片区域到节目号为nProgramNo作为背景,***开始***/
	AREARECT AreaRect;//区域坐标属性结构体变量
	AreaRect.left = 0;
	AreaRect.top = 0;
	AreaRect.width = ledWidth;
	AreaRect.height = ledHeight;
	printf("%d,%d>> left=%d,top=%d,width=%d,height=%d\n", nProgramNo, nAreaId, AreaRect.left, AreaRect.top, AreaRect.width, AreaRect.height);

	nResult = LV_AddImageTextArea(hProgram, nProgramNo, nAreaId, &AreaRect, 0);
	if (nResult)
	{
		char ErrStr[256];
		LV_GetError(nResult, 256, ErrStr);
		printf("%s\n", ErrStr);
		return;
	}

	PLAYPROP PlayProp;//显示属性
	PlayProp.DelayTime = 65535;
	PlayProp.InStyle = 0;
	PlayProp.OutStyle = 0;
	PlayProp.Speed = 1;

	//可以添加多个子项到图文区，如下添加可以选一个或多个添加
	printf("nProgramNo=%d,nAreaId=%d\n", nProgramNo, nAreaId);
	nResult = LV_AddFileToImageTextArea(hProgram, nProgramNo, nAreaId, "/home/dx/123.jpg", &PlayProp);
	//nResult=LV_AddFileToImageTextArea(hProgram,nProgramNo,nAreaId,"test.jpg",&PlayProp);

	if (nResult)
	{
		char ErrStr[256];
		LV_GetError(nResult, 256, ErrStr);
		printf("%s\n", ErrStr);
		return;
	}
	/***第三步添加一个图片区域***结束***/

	/***第四步添加两个个多行文本区域到节目号为nProgramNo***开始***/
	//区域坐标属性结构体变量
	for (int iArea = 0; iArea < 4; iArea++)
	{

		nAreaId++;

		AreaRect.left = 1;
		AreaRect.top = ledHeight * iArea / 4 + 1;
		AreaRect.width = ledWidth - 2;
		AreaRect.height = ledHeight / 4 - 1;
		if (AreaRect.height + AreaRect.top >= ledHeight)
			AreaRect.height = ledHeight - AreaRect.top - 1;

		printf("%d>> left=%d,top=%d,width=%d,height=%d\n", iArea, AreaRect.left, AreaRect.top, AreaRect.width, AreaRect.height);

		nResult = LV_AddImageTextArea(hProgram, nProgramNo, nAreaId, &AreaRect, 1);
		if (nResult)
		{
			char ErrStr[256];
			LV_GetError(nResult, 256, ErrStr);
			printf("%s\n", ErrStr);
			return;
		}

		FONTPROP FontProp; // 文字属性
		memset(&FontProp, 0, sizeof(FONTPROP));
		strcpy(FontProp.FontPath, "./font/simsun.ttc");
		FontProp.FontSize = 14;
		FontProp.FontColor = COLOR_RED;

		PlayProp.DelayTime = 3;
		PlayProp.InStyle = 0;
		PlayProp.OutStyle = 0;
		PlayProp.Speed = 32;
		char str[256];

		sprintf(str, "%d", std::time(nullptr) % 10000 + iArea);

		// 可以添加多个子项到图文区，如下添加可以选一个或多个添加
		// nResult=LV_AddStaticTextToImageTextArea(hProgram,nProgramNo,nAreaId,ADDTYPE_STRING,"上海灵信视觉技术股份有限公司",&FontProp,3,0,FALSE);
		nResult = LV_AddMultiLineTextToImageTextArea(hProgram, nProgramNo, nAreaId, ADDTYPE_STRING, str, &FontProp, &PlayProp, 0, FALSE); // 通过字符串添加一个多行文本到图文区，参数说明见声明注示
		// nResult=LV_AddMultiLineTextToImageTextArea(hProgram,nProgramNo,1,ADDTYPE_FILE,"test.txt",&FontProp,&PlayProp,0,FALSE);//通过txt文件添加一个多行文本到图文区，参数说明见声明注示
		if (nResult)
		{
			char ErrStr[256];
			LV_GetError(nResult, 256, ErrStr);
			printf("%s\n", ErrStr);
			return;
		}
	}



	/***第五步发送节目以及删除节目内存对象***开始***/
	nResult = LV_Send(&CommunicationInfo, hProgram);//发送，见函数声明注示
	LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
	if (nResult)
	{
		char ErrStr[256];
		LV_GetError(nResult, 256, ErrStr);
		printf("%s\n", ErrStr);
		return;
	}
	else
	{
		printf("发送成功\n");
	}
	/***第五步发送节目以及删除节目内存对象***结束***/
}
#endif

void LED_Server::test()
{
	std::lock_guard<std::mutex> lock(queue_mutex);
	for (auto a :clientNetWorkID)
	{
		printf("test--->%s\n", a.data());

#ifndef WIN32
		onTwoMutiTextAndBackground2(a.data(), 3, 80, 40, 3, 1);
#endif
	}
	
}
