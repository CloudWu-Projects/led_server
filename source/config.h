
#pragma once
#define MAX_PATH 256
#include "logLib.h"
#include "LED_lsprj.h"
#include <filesystem>
#include <functional>
namespace fs = std::filesystem;
class Config
{
public:
	static Config& instance() {
		static Config c;
		c.load();
		return c;
	}
	Config()
	{
		
	}
	
	
	struct MyTM
	{
		bool isValid = false;
		int tm_sec = 0;	 // seconds after the minute - [0, 60] including leap second
		int tm_min = 0;	 // minutes after the hour - [0, 59]
		int tm_hour = 0; // hours since midnight - [0, 23]
		
		bool getTimeFunc (char* szBuf1) {
			std::string s(szBuf1);
			auto pos1 = s.find(":");
			if (pos1 == std::string::npos)
				return false;
			auto pos2 = s.find(":", pos1 + 1);
			if (pos2 == std::string::npos)
				return false;
			szBuf1[pos1] = '\0';
			szBuf1[pos1] = '\0';
			szBuf1[pos2] = '\0';

			this->tm_hour = atoi(szBuf1);
			this->tm_min = atoi(szBuf1 + pos1 + 1);
			this->tm_sec = atoi(szBuf1 + pos2 + 1);
			this->isValid = true;


			return true;
		};
	};
	struct RestartParam {
		struct MyTM stime;
		struct tm lastRunTime;
		std::vector<std::string> m_IPs;
		bool isNeedRun(struct tm& local) {
			//       精确到分钟
			//判断 stime<当前时间
			//      lastRunTime
			auto isEqual = [&](MyTM& a) {
				if (local.tm_hour != a.tm_hour)return false;
				if (local.tm_min != a.tm_min)return false;
				if (local.tm_sec != a.tm_sec)return false;
				return true;
			};
			if (lastRunTime.tm_year == local.tm_year
				&& lastRunTime.tm_mon == local.tm_mon
				&& lastRunTime.tm_mday == local.tm_mday)
				return false;
			if (!isEqual(stime))return false;

			lastRunTime = local;
			//isFirstRun = false;
			return true;
		}

	};
	
	struct LEDParam {
		int ledType = 3;
		int ledWidth = 320; 
		int ledHeight = 40;
		int ledColor = 3;
		int ledGraylevel = 1;		
	};
	

	int httpPort = 8080;
	int ledPort = 10008;
	LEDParam ledParam;
	bool bWriteIni ;
	std::string m_ConfigPathA = ".\\config.ini";


	std::mutex led_lsprj_mutex;
	bool ReloadPGM()
	{
		std::lock_guard<std::mutex> lock(led_lsprj_mutex);
		if (led_lsprj.loadFile(pgmFIlePath.data()))
		{
			ledParam.ledType = led_lsprj.leds[0].LedType;
			ledParam.ledWidth = led_lsprj.leds[0].LedWidth;
			ledParam.ledHeight = led_lsprj.leds[0].LedHeight;
			ledParam.ledColor = led_lsprj.leds[0].LedColor;
			ledParam.ledGraylevel = led_lsprj.leds[0].LedGray;
			return true;
		}
		return false;
	}
	void foreach_PGM(std::function<void(LED_lsprj& _led_lsprj)>&& func) {

		std::lock_guard<std::mutex> lock(led_lsprj_mutex);
		func(led_lsprj);
	}
private:

	LED_lsprj led_lsprj;
	std::string pgmFIlePath = "20230427170156.lsprj";
	int getConfigInt(const char* szApp, const char* szKey, int defalut = 0)
	{
#ifdef WIN32
		if (bWriteIni)
			WritePrivateProfileStringA(szApp, szKey, "", m_ConfigPathA.data());
		return GetPrivateProfileIntA(szApp, szKey, defalut, m_ConfigPathA.data());
#else
		return defalut;
#endif
	}
	int getConfigString(const char* szApp, const char* szKey, char* retVal, const char* defaultV)
	{
#ifdef WIN32
		if (bWriteIni)
			WritePrivateProfileStringA(szApp, szKey, defaultV, m_ConfigPathA.data());
		return GetPrivateProfileStringA(szApp, szKey, defaultV, retVal, MAX_PATH, m_ConfigPathA.data());
#else
		return 0;
#endif
	}
	bool bLoaded = false;
	bool load()
	{
		if (bLoaded)return true;
		bLoaded = true;
		auto curpath = fs::current_path();
		//curpath.
		//strcat(szBuf1, "\\config.ini");
		curpath.append("config.ini");
		m_ConfigPathA = curpath.string();

		std::error_code error;
		if (!std::filesystem::exists(m_ConfigPathA, error)) //already have a file with the same name?
		{
			bWriteIni = true;
		}

		char szBuf1[MAX_PATH];
		int nret = 0;

		httpPort = getConfigInt("main", "httpport", 38080);
		ledPort = getConfigInt("main", "ledPort", 10008);


		ledParam.ledType = getConfigInt("LED", "ledType", ledParam.ledType);
		ledParam.ledWidth = getConfigInt("LED", "ledWidth", ledParam.ledWidth);
		ledParam.ledHeight = getConfigInt("LED", "ledHeight", ledParam.ledHeight);
		ledParam.ledColor = getConfigInt("LED", "ledColor", ledParam.ledColor);
		ledParam.ledGraylevel = getConfigInt("LED", "ledType", ledParam.ledGraylevel);

		if (getConfigString("LED", "lsprj_path", szBuf1, R"(D:\Cloud_wu\LED\led_server\20230427170156.lsprj)") && szBuf1[0] != '\0')
		{
			pgmFIlePath = szBuf1;
			ReloadPGM();
		}
		bLoaded = true;
		return true;
	}
};

#define IConfig  Config::instance()