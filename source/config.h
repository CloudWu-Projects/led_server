
#pragma once
#define MAX_PATH 256
#include "logLib.h"
#include "LED_lsprj.h"
#include "ledPgm.h"
#include <filesystem>
#include <functional>
#include "stringHelper.h"
//#define NEED_TCPSERVER_FOR_NEIMA 
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
	

	int httpPort = 11007;
	int ledSDKPort = 11008;
	int ledNeiMaPort= 11009;
	LEDParam ledParam;
	bool bWriteIni ;
	std::string m_ConfigPathA = ".\\ledServer_config.ini";

	std::string mainAppPath = "";
	std::mutex led_lsprj_mutex;
	bool ReloadPGM(const std::string &pgmPath)
	{
		std::lock_guard<std::mutex> lock(led_lsprj_mutex);

		for (auto led : leds)
		{
			for (auto p : led.programs)
			{
				p.areas.clear();
			}
			led.programs.clear();
		}
		leds.clear();
		if(!pgmPath.empty())
			pgmFIlePath = pgmPath;
		if (pgmFIlePath.extension() == ".lsprj")
		{
			if (led_lsprj.loadFile(pgmFIlePath.string().data(), leds))
			{
				ledParam.ledType = leds[0].LedType;
				ledParam.ledWidth = leds[0].LedWidth;
				ledParam.ledHeight = leds[0].LedHeight;
				ledParam.ledColor = leds[0].LedColor;
				ledParam.ledGraylevel = leds[0].LedGray;
				return true;
			}
		}
		return false;
	}
	void foreach_PGM(std::function<void(std::vector<LED>& leds)>&& func) {

		std::lock_guard<std::mutex> lock(led_lsprj_mutex);
		func(leds);
	}
private:
	LED_lsprj led_lsprj;
	std::vector<LED> leds;
	fs::path pgmFIlePath=("./single_area.lsprj");
	//IniParser iniParser;
	std::string GetValue(const std::string& key, const std::string& section = "")
	{
		//if (bWriteIni)
		//	iniParser.SetValue(key,"", section);
		return "";
		//return iniParser.GetValue(key, section);
	}
	
	// T = [bool, int, float]
	template<typename T>
	T Get(const std::string& key, const std::string& section = "", T defvalue = 0)
	{
		return defvalue;
	}



	bool bLoaded = false;
	bool load()
	{
		if (bLoaded)return true;
		bLoaded = true;
		auto curpath = fs::current_path();
		//curpath.
		//strcat(szBuf1, "\\config.ini");
		curpath.append("ledServer_config.ini");
		m_ConfigPathA = curpath.string();
		
		int nret = 0;

		httpPort = Get("httpport", "main", httpPort);
		ledSDKPort = Get("ledSDKPort", "main", ledSDKPort);
		ledNeiMaPort = Get("ledNeiMaPort", "main", ledNeiMaPort);
#ifndef NEED_TCPSERVER_FOR_NEIMA
		ledSDKPort = ledNeiMaPort;
#endif

		ledParam.ledType = Get("ledType", "LED", ledParam.ledType);
		ledParam.ledWidth = Get("ledWidth", "LED", ledParam.ledWidth);
		ledParam.ledHeight = Get("ledHeight", "LED", ledParam.ledHeight);
		ledParam.ledColor = Get("ledColor", "LED", ledParam.ledColor);
		ledParam.ledGraylevel = Get("ledType", "LED", ledParam.ledGraylevel);
		mainAppPath = GetValue("mainAppPath","main");
		auto aa = utf8_to_unicode(mainAppPath);
		auto lsprj_path = GetValue("lsprj_path");
		if (lsprj_path.empty())
			lsprj_path = R"(./single_area.lsprj)";
		ReloadPGM(lsprj_path);
		bLoaded = true;

		
		return true;
	}

	

};

#define IConfig  Config::instance()