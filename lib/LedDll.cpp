
#include "xLedDll.h"
#include <stdio.h>

#include <filesystem>

namespace fs = std::filesystem;




CLedDll::CLedDll()
{
	
}

CLedDll::~CLedDll()
{
	if(m_hDll!=NULL)
		FreeLibrary(m_hDll);	

}


BOOL CLedDll::InitDll()
{
#ifdef _WIN32
	#ifdef UNICODE
		#if defined(_AMD64_)
			#define dll_file_name L"LV_LED_64.DLL"
		#else
			#define dll_file_name L"LV_LED_32.DLL"
		#endif	
	#else
		#if defined(_AMD64_)
			#define dll_file_name "lv_led_MBCS_64.DLL"
		#else
			#define dll_file_name "lv_led_MBCS_32.DLL"
		#endif
	#endif
	m_hDll = LoadLibrary(dll_file_name);
	if (m_hDll == NULL)
	{
		printf("Load \"LV_LED.DLL\" Failed");
		return FALSE;
	}
#else
	auto soPath= fs::current_path().append("libledplayer7.so");
	m_hDll = dlopen(soPath.string().data(), RTLD_LAZY);
	if (m_hDll == NULL)
	{
		printf("Load \"LV_LED.DLL\" Failed  %s", dlerror());
		return FALSE;
	}
#endif
	
	
#ifndef _WIN32
	#define GetProcAddress dlsym
#endif

	LV_CreateProgramEx=(_LV_CreateProgramEx)GetProcAddress(m_hDll,"LV_CreateProgramEx");
	
	LV_AddProgram=(_LV_AddProgram)GetProcAddress(m_hDll,"LV_AddProgram");
//	LV_SetProgramTime=(_LV_SetProgramTime)GetProcAddress(m_hDll,"LV_SetProgramTime");
	LV_AddImageTextArea = (_LV_AddImageTextArea)GetProcAddress(m_hDll, "LV_AddImageTextArea");
	//	LV_AddFileToImageTextArea=(_LV_AddFileToImageTextArea)GetProcAddress(m_hDll,"LV_AddFileToImageTextArea");
	LV_AddSingleLineTextToImageTextArea = (_LV_AddSingleLineTextToImageTextArea)GetProcAddress(m_hDll, "LV_AddSingleLineTextToImageTextArea");
	//	LV_AddMultiLineTextToImageTextArea=(_LV_AddMultiLineTextToImageTextArea)GetProcAddress(m_hDll,"LV_AddMultiLineTextToImageTextArea");
	//	LV_AddStaticTextToImageTextArea=(_LV_AddStaticTextToImageTextArea)GetProcAddress(m_hDll,"LV_AddStaticTextToImageTextArea");
	LV_QuickAddSingleLineTextArea = (_LV_QuickAddSingleLineTextArea)GetProcAddress(m_hDll, "LV_QuickAddSingleLineTextArea");
		LV_AddDigitalClockArea=(_LV_AddDigitalClockArea)GetProcAddress(m_hDll,"LV_AddDigitalClockArea");
	//	LV_AddWaterBorder=(_LV_AddWaterBorder)GetProcAddress(m_hDll,"LV_AddWaterBorder");
		LV_AddTimeArea=(_LV_AddTimeArea)GetProcAddress(m_hDll,"LV_AddTimeArea");
	//	LV_AddVoiceArea=(_LV_AddVoiceArea)GetProcAddress(m_hDll,"LV_AddVoiceArea");
		LV_AddNeiMaArea=(_LV_AddNeiMaArea)GetProcAddress(m_hDll,"LV_AddNeiMaArea");
	LV_DeleteProgram = (_LV_DeleteProgram)GetProcAddress(m_hDll, "LV_DeleteProgram");

	LV_Send = (_LV_Send)GetProcAddress(m_hDll, "LV_Send");
	//	LV_MultiSendOne=(_LV_MultiSendOne)GetProcAddress(m_hDll,"LV_MultiSendOne");
	//	LV_MultiSend=(_LV_MultiSend)GetProcAddress(m_hDll,"LV_MultiSend");
	//	LV_SetBasicInfoEx=(_LV_SetBasicInfoEx)GetProcAddress(m_hDll,"LV_SetBasicInfoEx");
	//	LV_TestOnline=(_LV_TestOnline)GetProcAddress(m_hDll,"LV_TestOnline");
	//	LV_SetOEDA=(_LV_SetOEDA)GetProcAddress(m_hDll,"LV_SetOEDA");
	//	LV_AdjustTime=(_LV_AdjustTime)GetProcAddress(m_hDll,"LV_AdjustTime");
	//	LV_PowerOnOff=(_LV_PowerOnOff)GetProcAddress(m_hDll,"LV_PowerOnOff");

	LV_GetError = (_LV_GetError)GetProcAddress(m_hDll, "LV_GetError");

	LV_LedInitServer = (_LV_LedInitServer)GetProcAddress(m_hDll, "LV_LedInitServer");

	LV_LedShudownServer = (_LV_LedShudownServer)GetProcAddress(m_hDll, "LV_LedShudownServer");

	LV_RegisterLedServerCallback = (_LV_RegisterLedServerCallback)GetProcAddress(m_hDll, "LV_RegisterLedServerCallback");

	return TRUE;
}

