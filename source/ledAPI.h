#pragma once

#include "led.h"
#include "spdlog/spdlog.h"

#ifndef WIN32
#define _tcscpy strcpy
#endif

class LedAPI
{
public:
	HPROGRAM api_createProgram(int LedWidth, int LedHeight, int LedColor, int LedGray)
	{
		HPROGRAM hProgram = nullptr;
		int nResult = 0;
#ifdef WIN32
		hProgram = g_Dll->LV_CreateProgramEx(LedWidth, LedHeight, LedColor, LedGray, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误

		nResult = g_Dll->LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
#else

		hProgram = LV_CreateProgramEx(LedWidth, LedHeight, LedColor, LedGray, 0); // 注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误

		nResult = LV_AddProgram(hProgram, 0, 0, 1); // 添加一个节目，参数说明见函数声明注示
#endif
		if (nResult) {
			TCHAR ErrStr[256];

#ifdef WIN32
			g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
			LV_GetError(nResult, 256, ErrStr);		// 见函数声明注示
#endif
			SPDLOG_ERROR("api_createProgram error ,{} ", ErrStr);
			return nullptr;
		}
		return hProgram;
	}

	int api_addBackground(HPROGRAM hProgram, int ledWidth, int ledHeight, const std::string& backgroundImae)
	{
		PLAYPROP PlayProp; // 显示属性
		PlayProp.DelayTime = 65535;
		PlayProp.InStyle = 0;
		PlayProp.OutStyle = 0;
		PlayProp.Speed = 1;

		AREARECT AreaRect;//区域坐标属性结构体变量
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

		COMMUNICATIONINFO CommunicationInfo; // 定义一通讯参数结构体变量用于对设定的LED通讯，具体对此结构体元素赋值说明见COMMUNICATIONINFO结构体定义部份注示
		memset(&CommunicationInfo, 0, sizeof(COMMUNICATIONINFO));
		CommunicationInfo.LEDType = ledType;

		CommunicationInfo.SendType = 4;
		_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // 指定唯一网络ID

		int nResult = 0;

#ifdef WIN32
		nResult = g_Dll->LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
		g_Dll->LV_DeleteProgram(hProgram);					   // 删除节目内存对象，详见函数声明注示
#else
		nResult = LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
		LV_DeleteProgram(hProgram);					   // 删除节目内存对象，详见函数声明注示
#endif
		hProgram = nullptr;
		return nResult;
	}
	void api_getLastError(int nResult, TCHAR* ErrStr)
	{

#ifdef WIN32
		g_Dll->LV_GetError(nResult, 256, ErrStr); // 见函数声明注示
#else
		LV_GetError(nResult, 256, ErrStr);		// 见函数声明注示
#endif
	}
	void api_deleteProgram(HPROGRAM& hProgram)
	{
		if (hProgram)
		{
#ifdef WIN32
			g_Dll->LV_DeleteProgram(hProgram); // 删除节目内存对象，详见函数声明注示
#else
			LV_DeleteProgram(hProgram);			// 删除节目内存对象，详见函数声明注示
#endif
			hProgram = nullptr;
		}
	}
	int api_setBasicInfo(std::string& WnetworkID, int ledType, int ledWidth, int ledHeight, int colorType, int grayLevel)
	{
		int nResult;

		COMMUNICATIONINFO CommunicationInfo; // 定义一通讯参数结构体变量用于对设定的LED通讯，具体对此结构体元素赋值说明见COMMUNICATIONINFO结构体定义部份注示
		memset(&CommunicationInfo, 0, sizeof(COMMUNICATIONINFO));
		CommunicationInfo.LEDType = ledType;

		CommunicationInfo.SendType = 4;
		_tcscpy(CommunicationInfo.NetworkIdStr, (TCHAR*)WnetworkID.data()); // 指定唯一网络ID

		nResult = LV_SetBasicInfoEx(&CommunicationInfo, colorType, grayLevel, ledWidth, ledHeight);//设置屏参，具体函数参数说明查看文档
		return nResult;
	}

protected:

#ifdef WIN32
		CLedDll* g_Dll;
#endif
};