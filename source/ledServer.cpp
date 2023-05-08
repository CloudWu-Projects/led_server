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
	SPDLOG_DEBUG("LedServerCallback msg:%d \n", Msg);
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

std::tuple<int, std::string> LED_Server::createPGM_withLspj(std::string& showText, ExtSeting& extSetting)
{
	std::lock_guard<std::mutex> lock(queue_mutex);

	if (clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty\n");

	std::string retHtml = "";

	HPROGRAM hProgram = nullptr;
	for (auto WnetworkID : clientNetWorkID)
	{
		//auto ret = createAProgram(WnetworkID, showText,ledParam);
		if (hProgram == nullptr)
		{
			hProgram = createAProgram_withLspj(showText, extSetting);
		}
		if (hProgram == nullptr)
		{
			retHtml = "createAProgram_withLspj failed.";
			break;
		}
		auto ret = sendProgram(WnetworkID, hProgram);
		retHtml += std::get<1>(ret);
	}

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
		auto ret = createAProgram(WnetworkID, showText,extSetting,IConfig.ledParam);
		
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
		return std::make_tuple(nResult, fmt::format("{}>>LV_Send ErrStr :{}", networkID, to_byte_string(ErrStr)));
	}
	return std::make_tuple(0, fmt::format("{} setContent sucess", networkID));
}



std::tuple<int, std::string> LED_Server::createAProgram(NETWORKID WnetworkID, std::string& showText, ExtSeting& extSetting, const Config::LEDParam& ledParam)
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

		return std::make_tuple(nResult, fmt::format("{}>>LV_AddProgram ErrStr :%s\n", networkID, to_byte_string(ErrStr)));
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
	FontProp.FontSize = extSetting.FontSize;
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


		return std::make_tuple(nResult, fmt::format("{}>>LV_QuickAddSingleLineTextArea ErrStr :%s\n", networkID, to_byte_string(ErrStr)));
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

		return std::make_tuple(nResult, fmt::format("{}>>LV_Send ErrStr :%s\n", networkID, to_byte_string(ErrStr)));
	}
	return std::make_tuple(0, fmt::format("{} setContent sucess", networkID));
}

inline std::vector<std::string> split_string(const std::string& s, char delim)
{
	if (s.empty())
		return {};

	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}

HPROGRAM LED_Server::createAProgram_withLspj(std::string& showText, ExtSeting& extSetting)
{
	int nResult = 0;
	HPROGRAM hProgram = nullptr;																			 // 节目句柄

	auto stringArr = split_string(showText, ',');
	IConfig.foreach_PGM([&](LED_lsprj& led_lsprj)
		{
			for (auto led : led_lsprj.leds)
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
						LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#endif

						if (hProgram)
						{
#ifdef WIN32
							g_Dll->LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
#else
							LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
#endif
							hProgram = nullptr;
						}
						return;
					}
					for (auto area : p.areas)
					{
						AREARECT AreaRect{
						.left = area.AreaRect_Left,
						.top = area.AreaRect_Top,
						.width = area.AreaRect_Right - area.AreaRect_Left,
						.height = area.AreaRect_Bottom - area.AreaRect_Top,
						};

#ifdef WIN32
						nResult = g_Dll->LV_AddImageTextArea(hProgram, 0, area.AreaNo, &AreaRect, 1);
#else
						nResult = LV_AddImageTextArea(hProgram, 0, area.AreaNo, &AreaRect, 1);
#endif

						PLAYPROP PlayProp;//显示属性
						PlayProp.DelayTime = area.DelayTime;
						PlayProp.InStyle = area.InStyle;
						PlayProp.OutStyle = area.OutStyle;
						PlayProp.Speed = area.InSpeed;

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

						FontProp.FontSize = extSetting.FontSize;
						FontProp.FontColor =  area.FontColor;
						auto pShowText = showText.data();
						if (p.areas.size() > 1 && stringArr.size() == p.areas.size())
							pShowText = stringArr[area.AreaNo - 1].data();
#ifdef WIN32
						nResult = g_Dll->LV_AddSingleLineTextToImageTextArea(hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, &PlayProp);
#else
						nResult = LV_AddSingleLineTextToImageTextArea(hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, &PlayProp);
#endif
						//nResult = g_Dll->LV_AddStaticTextToImageTextArea(hProgram, 0, area.AreaNo, ADDTYPE_STRING, pShowText, &FontProp, area.DelayTime, 0, true);
						if (nResult)
						{
							TCHAR ErrStr[256];


#ifdef WIN32
							g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
							LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#endif
							if (hProgram)
							{

#ifdef WIN32
								g_Dll->LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
#else
								LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
#endif
								hProgram = nullptr;
							}
							return ;
						}
					}

					return ;
				}
			}
		});
	// 此处可自行判断有未创建成功，hProgram返回NULL失败，非NULL成功,一般不会失败


	return hProgram;
}
