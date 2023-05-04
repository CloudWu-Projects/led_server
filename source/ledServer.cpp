#include "ledServer.h"

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

LED_Server::LED_Server()
{

}

int LED_Server::start(int port)
{
	g_Dll = new CLedDll();
	g_Dll->InitDll();

	g_Dll->LV_RegisterLedServerCallback((SERVERINFOCALLBACK)LedServerCallback);
	int ret = g_Dll->LV_LedInitServer(port);

	printf("LV_LedInitServer ret=%d\n", ret);
	return ret;
}

std::string LED_Server::getNetWorkIDList()
{
	std::string htmlContent = "";
	{
		std::lock_guard<std::mutex> lock(queue_mutex);

		for (auto id : clientNetWorkID)
		{
			htmlContent += std::format("<li>{}</li>", to_byte_string(id));
		}
	}
	return htmlContent;
}

std::tuple<int, std::string> LED_Server::createAProgram(std::wstring& showText)
{
	std::lock_guard<std::mutex> lock(queue_mutex);

	if(clientNetWorkID.empty())
		return std::make_tuple(-1, "networkID is empty\n");

	std::string retHtml = "";

	for (auto WnetworkID : clientNetWorkID)
	{
		auto ret = createAProgram(WnetworkID, showText);
		retHtml += std::get<1>(ret);
	}
	
	return std::make_tuple(0, retHtml);
}

std::tuple<int, std::string> LED_Server::createAProgram(std::wstring WnetworkID, std::wstring& showText)
{
	std::string networkID = to_byte_string(WnetworkID);

	int nResult = 0;
	COMMUNICATIONINFO CommunicationInfo; // ����һͨѶ�����ṹ��������ڶ��趨��LEDͨѶ������Դ˽ṹ��Ԫ�ظ�ֵ˵����COMMUNICATIONINFO�ṹ�嶨�岿��עʾ
	ZeroMemory(&CommunicationInfo, sizeof(COMMUNICATIONINFO));
	CommunicationInfo.LEDType = m_ledType;

	CommunicationInfo.SendType = 4;
	_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // ָ��Ψһ����ID

	HPROGRAM hProgram;																			 // ��Ŀ���
	hProgram = g_Dll->LV_CreateProgramEx(m_ledWidth, m_ledHeight, m_ledColor, m_ledGraylevel, 0); // ע��˴�����߼���ɫ�����������������ε�����߼���ɫһ�£�������ʱ����ʾ����
	// �˴��������ж���δ�����ɹ���hProgram����NULLʧ�ܣ���NULL�ɹ�,һ�㲻��ʧ��

	nResult = g_Dll->LV_AddProgram(hProgram, 0, 0, 1); // ���һ����Ŀ������˵������������עʾ
	if (nResult)
	{
		TCHAR ErrStr[256];
		g_Dll->LV_GetError(nResult, 256, ErrStr); // ����������עʾ

		return std::make_tuple(nResult, std::format("{}>>LV_AddProgram ErrStr :%s\n", networkID, to_byte_string(ErrStr)));
	}
	AREARECT AreaRect; // �����������Խṹ�����
	AreaRect.left = 0;
	AreaRect.top = 0;
	AreaRect.width = m_ledWidth;
	AreaRect.height = m_ledHeight;

	FONTPROP FontProp; // ��������
	ZeroMemory(&FontProp, sizeof(FONTPROP));
	_tcscpy(FontProp.FontName, L"����");
	FontProp.FontSize = 12;
	FontProp.FontColor = COLOR_RED;

	nResult = g_Dll->LV_QuickAddSingleLineTextArea(hProgram, 0, 1, &AreaRect, ADDTYPE_STRING, showText.data(), &FontProp, 32); // ����ͨ���ַ����һ�������ı����򣬺�������������עʾ
	// nResult=g_Dll.LV_QuickAddSingleLineTextArea(hProgram,1,1,&AreaRect,ADDTYPE_FILE,_T("test.rtf"),NULL,4);//����ͨ��rtf�ļ����һ�������ı����򣬺�������������עʾ
	// nResult=g_Dll.LV_QuickAddSingleLineTextArea(hProgram,1,1,&AreaRect,ADDTYPE_FILE,_T("test.txt"),&FontProp,4);//����ͨ��txt�ļ����һ�������ı����򣬺�������������עʾ

	if (nResult)
	{
		TCHAR ErrStr[256];
		g_Dll->LV_GetError(nResult, 256, ErrStr); // ����������עʾ


		return std::make_tuple(nResult, std::format("{}>>LV_QuickAddSingleLineTextArea ErrStr :%s\n", networkID, to_byte_string(ErrStr)));
	}

	nResult = g_Dll->LV_Send(&CommunicationInfo, hProgram); // ���ͣ�����������עʾ
	g_Dll->LV_DeleteProgram(hProgram);					   // ɾ����Ŀ�ڴ���������������עʾ
	if (nResult)
	{
		TCHAR ErrStr[256];
		g_Dll->LV_GetError(nResult, 256, ErrStr); // ����������עʾ


		return std::make_tuple(nResult, std::format("{}>>LV_Send ErrStr :%s\n", networkID, to_byte_string(ErrStr)));
	}
	return std::make_tuple(0, std::format("{}>>���ͳɹ�",networkID));
}
