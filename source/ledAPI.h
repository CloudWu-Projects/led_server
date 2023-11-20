#pragma once

#include "led.h"
#include "spdlog/spdlog.h"

#ifndef WIN32
#define _tcscpy strcpy
#endif

class LedAPI
{
public:
	HPROGRAM api_createProgram(int LedWidth, int LedHeight, int LedColor, int LedGray,int SaveType)
	{
		HPROGRAM hProgram = nullptr;
		int nResult = 0;
		SPDLOG_DEBUG("api_createProgram(width:{},height:{},color:{},gray:{} savetype:{})"
		, LedWidth, LedHeight, LedColor, LedGray,SaveType);
#ifdef WIN32
		hProgram = g_Dll->LV_CreateProgramEx(LedWidth, LedHeight, LedColor, LedGray, SaveType); // ע��˴������߼���ɫ�����������������ε������߼���ɫһ�£�������ʱ����ʾ����

		nResult = g_Dll->LV_AddProgram(hProgram, 0, 0, 1); // ����һ����Ŀ������˵������������עʾ
#else

		hProgram = LV_CreateProgramEx(LedWidth, LedHeight, LedColor, LedGray, SaveType); // ע��˴������߼���ɫ�����������������ε������߼���ɫһ�£�������ʱ����ʾ����

		nResult = LV_AddProgram(hProgram, 0, 0, 1); // ����һ����Ŀ������˵������������עʾ
#endif
		if (nResult) {
			TCHAR ErrStr[256];

#ifdef WIN32
			g_Dll->LV_GetError(nResult, 256, ErrStr); // ����������עʾ
#else
			LV_GetError(nResult, 256, ErrStr);		// ����������עʾ
#endif
			SPDLOG_ERROR("api_createProgram error ,{} ", ErrStr);
			return nullptr;
		}
		return hProgram;
	}

	int api_addBackground(HPROGRAM hProgram, int ledWidth, int ledHeight, const std::string& backgroundImae)
	{
		PLAYPROP PlayProp; // ��ʾ����
		PlayProp.DelayTime = 65535;
		PlayProp.InStyle = 0;
		PlayProp.OutStyle = 0;
		PlayProp.Speed = 1;

		AREARECT AreaRect;//�����������Խṹ�����
		AreaRect.left = 0;
		AreaRect.top = 0;
		AreaRect.width = ledWidth;
		AreaRect.height = ledHeight;
		int nResult = 0;

#ifdef WIN32	
		nResult = g_Dll->LV_AddImageTextArea(hProgram, 0, 1, &AreaRect, 0);
#else
		nResult = LV_AddImageTextArea(hProgram, 0, 1, &AreaRect, 0);
#endif
		if (nResult == 0) {

#ifdef WIN32
			nResult = g_Dll->LV_AddFileToImageTextArea(hProgram, 0, 1, backgroundImae.data(), &PlayProp);
#else
			nResult = LV_AddFileToImageTextArea(hProgram, 0, 1, backgroundImae.data(), &PlayProp);
			//nResult = LV_AddFileToImageTextArea(hProgram, 0, 1, "/home/dx/123.jpg", &PlayProp);

#endif


		}
		return nResult;
	}
	int api_send(HPROGRAM& hProgram, int ledType, std::string& WnetworkID)
	{

		COMMUNICATIONINFO CommunicationInfo; // ����һͨѶ�����ṹ��������ڶ��趨��LEDͨѶ������Դ˽ṹ��Ԫ�ظ�ֵ˵����COMMUNICATIONINFO�ṹ�嶨�岿��עʾ
		memset(&CommunicationInfo, 0, sizeof(COMMUNICATIONINFO));
		CommunicationInfo.LEDType = ledType;

		CommunicationInfo.SendType = 4;
		_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // ָ��Ψһ����ID

		int nResult = 0;

#ifdef WIN32
		nResult = g_Dll->LV_Send(&CommunicationInfo, hProgram); // ���ͣ�����������עʾ
		g_Dll->LV_DeleteProgram(hProgram);					   // ɾ����Ŀ�ڴ���������������עʾ
#else
		nResult = LV_Send(&CommunicationInfo, hProgram); // ���ͣ�����������עʾ
		LV_DeleteProgram(hProgram);					   // ɾ����Ŀ�ڴ���������������עʾ
#endif
		hProgram = nullptr;
		return nResult;
	}
	void api_getLastError(int nResult, TCHAR* ErrStr)
	{

#ifdef WIN32
		g_Dll->LV_GetError(nResult, 256, ErrStr); // ����������עʾ
#else
		LV_GetError(nResult, 256, ErrStr);		// ����������עʾ
#endif
	}
	void api_deleteProgram(HPROGRAM& hProgram)
	{
		if (hProgram)
		{
#ifdef WIN32
			g_Dll->LV_DeleteProgram(hProgram); // ɾ����Ŀ�ڴ���������������עʾ
#else
			LV_DeleteProgram(hProgram);			// ɾ����Ŀ�ڴ���������������עʾ
#endif
			hProgram = nullptr;
		}
	}
	int api_setBasicInfo(std::string& WnetworkID, int ledType, int ledWidth, int ledHeight, int colorType, int grayLevel)
	{
		int nResult;

		COMMUNICATIONINFO CommunicationInfo; // ����һͨѶ�����ṹ��������ڶ��趨��LEDͨѶ������Դ˽ṹ��Ԫ�ظ�ֵ˵����COMMUNICATIONINFO�ṹ�嶨�岿��עʾ
		memset(&CommunicationInfo, 0, sizeof(COMMUNICATIONINFO));
		CommunicationInfo.LEDType = ledType;

		CommunicationInfo.SendType = 4;
		_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // ָ��Ψһ����ID

		nResult = LV_SetBasicInfoEx(&CommunicationInfo, colorType, grayLevel, ledWidth, ledHeight);//�������Σ����庯������˵���鿴�ĵ�
		return nResult;
	}

protected:

#ifdef WIN32
		CLedDll* g_Dll;
#endif
};