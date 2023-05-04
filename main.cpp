
#include "httplib.h"
#include <stdio.h>
#include "LedDll.h"

#define TRACE printf

#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <format>

#include "stringHelper.h"
std::mutex queue_mutex;
std::set<std::wstring> clientNetWorkID;

int LedServerCallback(int Msg, int wParam, void* lParam)
{
	printf("LedServerCallback msg:%d \n", Msg);
	switch (Msg)
	{
	case LV_MSG_CARD_ONLINE:
	{
		LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;
		TRACE("LV_MSG_CARD_ONLINE\n");
		TRACE("pCardInfo->port:%d\n", pCardInfo->port);
		wprintf(L"pCardInfo->ipStr:%s\n", pCardInfo->ipStr);
		wprintf(L"pCardInfo->networkIdStr:%s\n", pCardInfo->networkIdStr);
		{
			std::lock_guard<std::mutex> lock(queue_mutex);
			clientNetWorkID.emplace(pCardInfo->networkIdStr);
		}
	}
	break;
	case LV_MSG_CARD_OFFLINE:
	{
		LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;
		TRACE("LV_MSG_CARD_OFFLINE\n");
		TRACE("pCardInfo->port:%d\n", pCardInfo->port);
		wprintf(L"pCardInfo->ipStr:%s\n", pCardInfo->ipStr);
		wprintf(L"pCardInfo->networkIdStr:%s\n", pCardInfo->networkIdStr);
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

class LED_Server
{
public:
	int start(int port = 10008)
	{
		g_Dll.InitDll();

		g_Dll.LV_RegisterLedServerCallback((SERVERINFOCALLBACK)LedServerCallback);
		int ret = g_Dll.LV_LedInitServer(port);

		printf("LV_LedInitServer ret=%d\n", ret);
		return ret;
	}
	void createAProgram(std::wstring& showText)
	{
		std::wstring networkID = L"";
		{
			std::lock_guard<std::mutex> lock(queue_mutex);

			if (auto it = clientNetWorkID.begin(); it != clientNetWorkID.end())
			{
				networkID = *it;
			}
		}
		if (networkID.empty())
		{
			printf("networkID is empty\n");
			return;
		}
		int nResult = 0;
		COMMUNICATIONINFO CommunicationInfo; // 定义一通讯参数结构体变量用于对设定的LED通讯，具体对此结构体元素赋值说明见COMMUNICATIONINFO结构体定义部份注示
		ZeroMemory(&CommunicationInfo, sizeof(COMMUNICATIONINFO));
		CommunicationInfo.LEDType = m_ledType;

		CommunicationInfo.SendType = 4;
		_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)networkID.data()); // 指定唯一网络ID

		HPROGRAM hProgram;																			 // 节目句柄
		hProgram = g_Dll.LV_CreateProgramEx(m_ledWidth, m_ledHeight, m_ledColor, m_ledGraylevel, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
		// 此处可自行判断有未创建成功，hProgram返回NULL失败，非NULL成功,一般不会失败

		nResult = g_Dll.LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
		if (nResult)
		{
			TCHAR ErrStr[256];
			g_Dll.LV_GetError(nResult, 256, ErrStr); // 见函数声明注示

			printf("ErrStr :%s\n", ErrStr);
			return;
		}
		AREARECT AreaRect; // 区域坐标属性结构体变量
		AreaRect.left = 0;
		AreaRect.top = 0;
		AreaRect.width = m_ledWidth;
		AreaRect.height = m_ledHeight;

		FONTPROP FontProp; // 文字属性
		ZeroMemory(&FontProp, sizeof(FONTPROP));
		_tcscpy(FontProp.FontName, L"宋体");
		FontProp.FontSize = 12;
		FontProp.FontColor = COLOR_RED;

		nResult = g_Dll.LV_QuickAddSingleLineTextArea(hProgram, 0, 1, &AreaRect, ADDTYPE_STRING, showText.data(), &FontProp, 32); // 快速通过字符添加一个单行文本区域，函数见函数声明注示
		// nResult=g_Dll.LV_QuickAddSingleLineTextArea(hProgram,1,1,&AreaRect,ADDTYPE_FILE,_T("test.rtf"),NULL,4);//快速通过rtf文件添加一个单行文本区域，函数见函数声明注示
		// nResult=g_Dll.LV_QuickAddSingleLineTextArea(hProgram,1,1,&AreaRect,ADDTYPE_FILE,_T("test.txt"),&FontProp,4);//快速通过txt文件添加一个单行文本区域，函数见函数声明注示

		if (nResult)
		{
			TCHAR ErrStr[256];
			g_Dll.LV_GetError(nResult, 256, ErrStr); // 见函数声明注示

			printf("ErrStr :%s\n", ErrStr);
			return;
		}

		nResult = g_Dll.LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
		g_Dll.LV_DeleteProgram(hProgram);					   // 删除节目内存对象，详见函数声明注示
		if (nResult)
		{
			TCHAR ErrStr[256];
			g_Dll.LV_GetError(nResult, 256, ErrStr); // 见函数声明注示

			wprintf(L"ErrStr :%s\n", ErrStr);
			return;
		}
		else
		{
			wprintf(L"\n发送成功\n");
		}
	}
	int m_ledType = 3;
	int m_ledWidth = 320, m_ledHeight = 40, m_ledColor = 3, m_ledGraylevel = 1;

private:
	CLedDll g_Dll;
};


void startHttpServer(LED_Server& ledServer)
{
	// HTTP
	httplib::Server svr;

	svr.Get("/hi", [&ledServer](const httplib::Request& req, httplib::Response& res)
		{
			std::wstring sendValue = L"a发送成功aa";


			if (req.has_param("key")) {
				sendValue = to_wide_string(req.get_param_value("key"));				
			}
			ledServer.createAProgram(sendValue);
			std::string htmlContent = "";
			{
				std::lock_guard<std::mutex> lock(queue_mutex);

				for(auto id: clientNetWorkID)
				{
					htmlContent += std::format("<li>{}</li>", to_byte_string(id));
				}
			}
			res.set_content(htmlContent, "text/plain"); });

	svr.listen("0.0.0.0", 8080);
}

int main()
{
	printf("\naaaaaaaaaaaaaa\n");
	LED_Server ledServer;
	ledServer.start(10008);
	startHttpServer(ledServer);
	getchar();
	return 0;
}