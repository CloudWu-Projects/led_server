/*
 *****************************************************************************************************************
 *													  LED SDK 7.0
 *
 *														 胡伟
 *	
 *
 *										(C) Copyright 2006 - 2021, LISTEN VISION
 *												   All Rights Reserved
 *														  
 *****************************************************************************************************************
 */

#pragma once


#include "ledWin.h"
#include <tchar.h>


class CLedDll
{
private:
	HMODULE m_hDll;
public:
	CLedDll();
	~CLedDll();


	_LV_CreateProgramEx						LV_CreateProgramEx;
	_LV_AddProgram							LV_AddProgram;
//	_LV_SetProgramTime						LV_SetProgramTime;
	_LV_AddImageTextArea					LV_AddImageTextArea;
//	_LV_AddFileToImageTextArea				LV_AddFileToImageTextArea;
	_LV_AddSingleLineTextToImageTextArea	LV_AddSingleLineTextToImageTextArea;
//	_LV_AddMultiLineTextToImageTextArea		LV_AddMultiLineTextToImageTextArea;
//	_LV_AddStaticTextToImageTextArea		LV_AddStaticTextToImageTextArea;
	_LV_QuickAddSingleLineTextArea			LV_QuickAddSingleLineTextArea;
	_LV_AddDigitalClockArea					LV_AddDigitalClockArea;
	_LV_AddTimeArea							LV_AddTimeArea;
//	_LV_AddWaterBorder						LV_AddWaterBorder;	
//	_LV_AddVoiceArea						LV_AddVoiceArea;
	_LV_AddNeiMaArea						LV_AddNeiMaArea;
	_LV_DeleteProgram						LV_DeleteProgram;
//	
	_LV_Send								LV_Send;
//	_LV_MultiSendOne						LV_MultiSendOne;
//	_LV_MultiSend							LV_MultiSend;
//	
//	
//	_LV_SetBasicInfoEx						LV_SetBasicInfoEx;
//	_LV_TestOnline							LV_TestOnline;
//	_LV_SetOEDA								LV_SetOEDA;
//	_LV_AdjustTime							LV_AdjustTime;
//	_LV_PowerOnOff							LV_PowerOnOff;

	_LV_GetError							LV_GetError;

	_LV_LedInitServer						LV_LedInitServer;
	_LV_LedShudownServer					LV_LedShudownServer;
	_LV_RegisterLedServerCallback			LV_RegisterLedServerCallback;

	BOOL InitDll();
};

