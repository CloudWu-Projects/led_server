#include "ledServer.h"

#include "led.h"

#define TRACE printf

#include <string>
#include <set>
#include <thread>
#include <mutex>
//#include <format>


#include "stringHelper.h"
#include "logLib.h"
std::mutex queue_mutex;
#ifdef UNICODE
std::set<std::wstring> clientNetWorkID;
#else
std::set<std::string> clientNetWorkID;
#endif

#ifndef WIN32
#define _tcscpy strcpy
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
		//((CDemoDlg *)AfxGetApp()->m_pMainWnd)->ComboboxAddString(FALSE,pCardInfo->networkIdStr);
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

std::tuple<int, std::string> LED_Server::createPGM_withLspj(const std::string &ledids, const std::string &empty_plot, const std::string &pgmfilepath, ExtSeting &extSetting)
{
	std::lock_guard<std::mutex> lock(queue_mutex);
	if (clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty\n");
	std::vector<LED> leds;

	LED_lsprj led_lsprj;
	if(!led_lsprj.loadFile(pgmfilepath.data(),leds))
	{
		return std::make_tuple(-2, fmt::format("load file[{}] failed.",pgmfilepath));
	}
	std::string retHtml = "";
	m_hProgram = nullptr;
	
	auto vecLedids = split_string(ledids, ',');
	std::string noinItem="";
	std::string sucessItem="";
	for (auto ledid : vecLedids)
	{
		if (!clientNetWorkID.contains(ledid))
		{
			if(noinItem.length()>0)
				noinItem+=",";
			noinItem += "\"" + ledid + "\"";
			continue;
		}
		if (m_hProgram == nullptr)
		{	
			int ledType = leds[0].LedType;
			int ledWidth = leds[0].LedWidth;
			int ledHeight = leds[0].LedHeight;
			int ledColor = leds[0].LedColor;
			int ledGraylevel = leds[0].LedGray;
			m_hProgram = createAProgram_withLspj(empty_plot, leds,&extSetting);
		}
		if (m_hProgram == nullptr)
		{
			retHtml = "createAProgram_withLspj failed.";
			break;
		}
		auto ret = sendProgram(ledid, m_hProgram);
		if(sucessItem.length()>0)
			sucessItem+=",";
		sucessItem += "\"" + std::get<1>(ret);
		sucessItem += "\"";
		
	}
	std::string noInHtml="\"noin\":["+noinItem+"]";
	
	std::string sucessHtml="\"sucess\":["+sucessItem+"]";
	
	retHtml = noInHtml+","+sucessHtml;

#ifdef WIN32
	g_Dll->LV_DeleteProgram(m_hProgram);					   // 删除节目内存对象，详见函数声明注示
#else
	LV_DeleteProgram(m_hProgram);					   // 删除节目内存对象，详见函数声明注示
#endif	
	m_hProgram = nullptr;

	return std::make_tuple(0, retHtml);
}
std::tuple<int, std::string> LED_Server::createPGM_withLspj(bool isJson,std::string& showText, ExtSeting& extSetting)
{
	std::lock_guard<std::mutex> lock(queue_mutex);

	if (clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty\n");

	std::string retHtml = "";

	m_hProgram = nullptr;
	for (auto WnetworkID : clientNetWorkID)
	{
		//auto ret = createAProgram(WnetworkID, showText,ledParam);
		if (m_hProgram == nullptr)
		{
			IConfig.foreach_PGM([&](auto &leds){				
				m_hProgram = isJson ?
					createAProgram_withJson(showText,leds,&extSetting)
					: createAProgram_withLspj(showText,leds,&extSetting);				
			});
		}
		if (m_hProgram == nullptr)
		{
			retHtml = "createAProgram_withLspj failed.";
			break;
		}
		auto ret = sendProgram(WnetworkID, m_hProgram);
		retHtml += std::get<1>(ret);
		retHtml += "\n";
	}

#ifdef WIN32
	g_Dll->LV_DeleteProgram(m_hProgram);					   // 删除节目内存对象，详见函数声明注示
#else
	LV_DeleteProgram(m_hProgram);					   // 删除节目内存对象，详见函数声明注示
#endif	
	m_hProgram = nullptr;

	return std::make_tuple(0, retHtml);
}

std::tuple<int, std::string> LED_Server::create_onPGM_byCode(std::string& showText, ExtSeting& extSetting)
{
	std::lock_guard<std::mutex> lock(queue_mutex);

	if (clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty\n");

	std::string retHtml = "";
	HPROGRAM hProgram = nullptr;
	for (auto WnetworkID : clientNetWorkID)
	{
		auto ret = createAProgram(WnetworkID, showText,IConfig.ledParam,&extSetting);
		
		retHtml += std::get<1>(ret);
	}

	return std::make_tuple(0, retHtml);
}

std::tuple<int, std::string> LED_Server::sendProgram(NETWORKID WnetworkID, HPROGRAM hProgram)
{
	std::string networkID = to_byte_string(WnetworkID);

	int nResult = 0;
	COMMUNICATIONINFO CommunicationInfo; // 定义一通讯参数结构体变量用于对设定的LED通讯，具体对此结构体元素赋值说明见COMMUNICATIONINFO结构体定义部份注示
	memset(&CommunicationInfo,0, sizeof(COMMUNICATIONINFO));
	CommunicationInfo.LEDType = IConfig.ledParam.ledType;

	CommunicationInfo.SendType = 4;
	_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // 指定唯一网络ID

#ifdef WIN32
	nResult = g_Dll->LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示	
#else
	nResult = LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
#endif	
	
	if (nResult)
	{
		TCHAR ErrStr[256];
#ifdef WIN32
		g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
		LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#endif
		return std::make_tuple(nResult, fmt::format("{}>>LV_Send ErrStr :{}", networkID, to_byte_string(ErrStr)));
	}
	return std::make_tuple(0, fmt::format("{} setContent sucess", networkID));
}



std::tuple<int, std::string> LED_Server::createAProgram(NETWORKID WnetworkID, std::string& showText, const Config::LEDParam& ledParam,ExtSeting *m_extSetting)
{
	std::string networkID = to_byte_string(WnetworkID);

	int nResult = 0;
	COMMUNICATIONINFO CommunicationInfo; // 定义一通讯参数结构体变量用于对设定的LED通讯，具体对此结构体元素赋值说明见COMMUNICATIONINFO结构体定义部份注示
	memset(&CommunicationInfo,0, sizeof(COMMUNICATIONINFO));
	CommunicationInfo.LEDType = ledParam.ledType;

	CommunicationInfo.SendType = 4;
	_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // 指定唯一网络ID

	HPROGRAM hProgram;																			 // 节目句柄
#ifdef WIN32
	hProgram = g_Dll->LV_CreateProgramEx(ledParam.ledWidth, ledParam.ledHeight, ledParam.ledColor, ledParam.ledGraylevel, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
	nResult = g_Dll->LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
#else
	hProgram = LV_CreateProgramEx(ledParam.ledWidth, ledParam.ledHeight, ledParam.ledColor, ledParam.ledGraylevel, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
	nResult = LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
#endif
	// 此处可自行判断有未创建成功，hProgram返回NULL失败，非NULL成功,一般不会失败

	
	if (nResult)
	{
		TCHAR ErrStr[256];

#ifdef WIN32
		g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
		LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#endif

		return std::make_tuple(nResult, fmt::format("{}>>LV_AddProgram ErrStr :{}\n", networkID, to_byte_string(ErrStr)));
	}
	AREARECT AreaRect; // 区域坐标属性结构体变量
	AreaRect.left = 0;
	AreaRect.top = 0;
	AreaRect.width = ledParam.ledWidth;
	AreaRect.height = ledParam.ledHeight;

	FONTPROP FontProp; // 文字属性
	memset(&FontProp,0, sizeof(FONTPROP));
#ifdef UNICODE
	_tcscpy(FontProp.FontName, L"宋体");
#else 
#ifdef WIN32
	strcpy(FontProp.FontName, "宋体");
#else
	strcpy(FontProp.FontPath, "./simsun.ttc");
#endif
#endif
	FontProp.FontSize = m_extSetting->FontSize;
	FontProp.FontColor = COLOR_RED;

#ifdef WIN32
	nResult = g_Dll->LV_QuickAddSingleLineTextArea(hProgram, 0, 1, &AreaRect, ADDTYPE_STRING, showText.data(), &FontProp, 32); // 快速通过字符添加一个单行文本区域，函数见函数声明注示
#else
	nResult = LV_QuickAddSingleLineTextArea(hProgram, 0, 1, &AreaRect, ADDTYPE_STRING, showText.data(), &FontProp, 32); // 快速通过字符添加一个单行文本区域，函数见函数声明注示
#endif
	// nResult=g_Dll.LV_QuickAddSingleLineTextArea(hProgram,1,1,&AreaRect,ADDTYPE_FILE,_T("test.rtf"),NULL,4);//快速通过rtf文件添加一个单行文本区域，函数见函数声明注示
	// nResult=g_Dll.LV_QuickAddSingleLineTextArea(hProgram,1,1,&AreaRect,ADDTYPE_FILE,_T("test.txt"),&FontProp,4);//快速通过txt文件添加一个单行文本区域，函数见函数声明注示

	if (nResult)
	{
		TCHAR ErrStr[256];
#ifdef WIN32
		g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
		LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#endif


		return std::make_tuple(nResult, fmt::format("{}>>LV_QuickAddSingleLineTextArea ErrStr :{}\n", networkID, to_byte_string(ErrStr)));
	}
#ifdef WIN32
	nResult = g_Dll->LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
	g_Dll->LV_DeleteProgram(hProgram);					   // 删除节目内存对象，详见函数声明注示
#else
	nResult = LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
	LV_DeleteProgram(hProgram);					   // 删除节目内存对象，详见函数声明注示
#endif
	
	if (nResult)
	{
		TCHAR ErrStr[256];

#ifdef WIN32
		g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
		LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#endif

		return std::make_tuple(nResult, fmt::format("{}>>LV_Send ErrStr :{}\n", networkID, to_byte_string(ErrStr)));
	}
	return std::make_tuple(0, fmt::format("{} setContent sucess", networkID));
}

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

template <typename ValType>
int32_t fill(ValType& _val, const char* source,  std::string& errorMsg, rapidjson::Document& document)
{
	rapidjson::Pointer pointer(source);

	if (rapidjson::Value* v1 = rapidjson::GetValueByPointer(document, pointer))
	{
		if constexpr (std::is_same_v < ValType, std::string>)
		{
			if (v1->IsString())
			{
				_val = v1->GetString();
				return 0;
			}
		}
		else if constexpr (std::is_integral_v<ValType>)
		{
			if (v1->IsString())
				_val = std::stoull(v1->GetString());
			else
				_val = v1->GetUint64();
			return 0;
		}
		else if constexpr (std::is_floating_point_v<ValType>)
		{
			if (v1->IsString())
				_val = std::stod(v1->GetString());
			else
				_val = v1->GetDouble();
			return 0;
		}
	}
	errorMsg = fmt::format("parse error, {} not found", source);
	return -1;
}

HPROGRAM LED_Server::createAProgram_withJson(const std::string& showText,std::vector<LED> &leds,ExtSeting *m_extSetting)
{
	int nResult = 0;
	HPROGRAM hProgram = nullptr;																			 // 节目句柄

	rapidjson::Document document;
	document.Parse((showText).c_str());
	if (document.HasParseError())
	{	
		return nullptr;
	}
	//auto a = document["pgm"];
	return HPROGRAM();
}

HPROGRAM LED_Server::createAProgram_withLspj(const std::string& showText,std::vector<LED> &leds,ExtSeting *m_extSetting)
{
	int nResult = 0;
	HPROGRAM hProgram = nullptr;																			 // 节目句柄

	auto stringArr = split_string(showText, ',');

	for (auto led : leds)
	{
		for (auto p : led.programs)
		{

#ifdef WIN32
			hProgram = g_Dll->LV_CreateProgramEx(led.LedWidth, led.LedHeight, led.LedColor, led.LedGray, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误

			nResult = g_Dll->LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
#else

			hProgram = LV_CreateProgramEx(led.LedWidth, led.LedHeight, led.LedColor, led.LedGray, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误

			nResult = LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
#endif
			if (nResult)
			{
				TCHAR ErrStr[256];

#ifdef WIN32
				g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
				LV_GetError(nResult, 256, ErrStr);		// 见函数声明注示
#endif

				if (hProgram)
				{
#ifdef WIN32
					g_Dll->LV_DeleteProgram(hProgram); // 删除节目内存对象，详见函数声明注示
#else
					LV_DeleteProgram(hProgram);			// 删除节目内存对象，详见函数声明注示
#endif
					hProgram = nullptr;
					
				}
				return hProgram;
			}
			m_hProgram = hProgram;
			for (auto area : p.areas)
			{

				auto pShowText = showText.data();
				if (p.areas.size() > 1 && stringArr.size() == p.areas.size())
					pShowText = stringArr[area.AreaNo - 1].data();
				if (area.areaType == Area::SINGLELINEAREA)
					nResult = createSingleLineArea(area, pShowText, m_extSetting);
				else if (area.areaType == Area::TIME_AREA)
					nResult = createTimeClockArea(area, m_extSetting);
				else if (area.areaType == Area::NEIMA_AREA)
					nResult = createNeimaArea(area, pShowText, m_extSetting);

				if (nResult)
				{
					TCHAR ErrStr[256];
#ifdef WIN32
					g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
					LV_GetError(nResult, 256, ErrStr);	// 见函数声明注示
#endif
					if (hProgram)
					{
#ifdef WIN32
						g_Dll->LV_DeleteProgram(hProgram); // 删除节目内存对象，详见函数声明注示
#else
						LV_DeleteProgram(hProgram);		// 删除节目内存对象，详见函数声明注示
#endif
						hProgram = nullptr;
						return hProgram;
					}
				}
			}

			return hProgram;
		}
	}

	return hProgram;
}
int LED_Server::createSingleLineArea(Area&area,const char*pShowText,ExtSeting *m_extSetting)
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

	FontProp.FontSize = m_extSetting->FontSize;
	FontProp.FontColor = area.singleLineArea.FontColor;

#ifdef WIN32
	nResult = g_Dll->LV_AddSingleLineTextToImageTextArea(m_hProgram, m_nProgramNo, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, &PlayProp);
#else
	nResult = LV_AddSingleLineTextToImageTextArea(m_hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, &PlayProp);
#endif
	//nResult = g_Dll->LV_AddStaticTextToImageTextArea(hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, area.DelayTime, 0, true);
	
	return nResult;
}

int LED_Server::createTimeClockArea( Area& area,ExtSeting *m_extSetting)
{
	//添加一个数字时钟区域 

	

#ifdef WIN32
	strcpy(area.clockIfo.ShowStrFont.FontName, "宋体");
#else
	strcpy(area.clockIfo.ShowStrFont.FontPath, "./font/simsun.ttc");
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

int LED_Server::createNeimaArea(  Area& area,const char* pShowText,ExtSeting *m_extSetting)
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
