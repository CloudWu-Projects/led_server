#include "LED_lsprj.h"
#include "tinyxml2.h"
#include "cpp-base64/base64.h"
#include "ledPgm.h"
//#include "logLib.h"

bool LED_lsprj::loadFile(const char*filePath,std::vector<LED>&leds) {
	tinyxml2::XMLDocument doc;
	if (auto error = doc.LoadFile(filePath);
		error != tinyxml2::XMLError::XML_SUCCESS)
	{
		//SPDLOG_ERROR("LoadFile xml failed. {}", error);
		return false;
	}
	parse(&doc, leds);
	return true;
}

bool LED_lsprj::loadMem(const char* pText,std::vector<LED>&leds) {

	tinyxml2::XMLDocument doc;
	if (auto error = doc.Parse(pText);
		error != tinyxml2::XMLError::XML_SUCCESS)
	{
		//SPDLOG_ERROR("loadMem xml failed. {}", error);
		return false;
	}
	parse(&doc, leds);
	return true;
}

void parseFontColor_size_From_RTF(std::string &RTFtext,int &color,int &size)
{
	/*
	TODO: RTFtext is
			{\rtf1\fbidis\ansi\ansicpg936\deff0\nouicompat\rtldoc{\fonttbl{\f0\fswiss\fprq2\fcharset134 \'cb\'ce\'cc\'e5;}}
{\colortbl ;\red0\green255\blue0;\red255\green0\blue0;}
{\*\generator Riched20 10.0.22621}\viewkind4\uc1
\pard\ltrpar\cf1\f0\fs32\lang2052\'d7\'d6\'c4\'bb1\cf2\fs24\par
}
	parser RTFtext to get color
	\fs32  is  fontsize
			*/

	do
	{
		size_t redStart = RTFtext.find("\\red");
		size_t greenStart = RTFtext.find("\\green");
		size_t blueStart = RTFtext.find("\\blue");
		if (redStart == std::string::npos || greenStart == std::string::npos || blueStart == std::string::npos)
		{
			// Malformed color table
			break;
		}
		redStart += 4;
		greenStart += 6;
		blueStart += 5;
		int red = std::stoi(RTFtext.substr(redStart, greenStart - redStart - 1));
		int green = std::stoi(RTFtext.substr(greenStart, blueStart - greenStart - 1));
		int blue = std::stoi(RTFtext.substr(blueStart));
		color = (blue << 16) | (green << 8) | red;
		// BBGGRR
	} while (0);
	size_t fontsizeEnd = RTFtext.find("\\lang");
	if (fontsizeEnd == std::string::npos)
	{
		// Malformed color table
		return;
	}
	size_t fontsizeStart = RTFtext.rfind("\\fs",fontsizeEnd);
	if (fontsizeStart == std::string::npos)
	{
		// Malformed color table
		return;
	}
	fontsizeStart += 3;
	//printf("fontsizeStart %s\n",RTFtext.substr(fontsizeStart,fontsizeEnd-fontsizeStart).data());
	size = std::stoi(RTFtext.substr(fontsizeStart,fontsizeEnd-fontsizeStart));
}
void LED_lsprj::parse(tinyxml2::XMLDocument* doc,std::vector<LED>&leds)
{
	/*
	* LEDS>>>LED>>
	ModuleWidth="80"   ModuleHeight="40"
	WidthModuleNum="1" HeightModuleNum="1"
	LedWidth="80"   LedHeight="40"

	* LEDS>>>LED>>Program>>
	<Area AreaNo="1"
	AreaRect_Left="0" AreaRect_Top="0"
	AreaRect_Right="40" AreaRect_Bottom="40" >


	*/
	
	
	auto pLed = doc->FirstChildElement("LEDS")->FirstChildElement("LED");
	LED led;
	pLed->FindAttribute("ModuleWidth")->QueryIntValue(&led.ModuleWidth);
	pLed->FindAttribute("ModuleHeight")->QueryIntValue(&led.ModuleHeight);
	pLed->FindAttribute("WidthModuleNum")->QueryIntValue(&led.WidthModuleNum);
	pLed->FindAttribute("HeightModuleNum")->QueryIntValue(&led.HeightModuleNum);
	pLed->FindAttribute("LedWidth")->QueryIntValue(&led.LedWidth);
	pLed->FindAttribute("LedHeight")->QueryIntValue(&led.LedHeight);
	pLed->FindAttribute("LedType")->QueryIntValue(&led.LedType);
	pLed->FindAttribute("LedColor")->QueryIntValue(&led.LedColor); 
	led.LedColor++;
	pLed->FindAttribute("LedGray")->QueryIntValue(&led.LedGray); 
	led.LedGray++;
	
	Program program;
	for (auto pArea = pLed->FirstChildElement("Program")->FirstChildElement("Area"); pArea != nullptr; pArea = pArea->NextSiblingElement())
	{
		Area area;
		pArea->FindAttribute("AreaNo")->QueryIntValue(&area.AreaNo);
		area.AreaName=std::string(pArea->FindAttribute("AreaName")->Value());
		
		pArea->FindAttribute("AreaRect_Left")->QueryIntValue(&area.AreaRect.left);
		pArea->FindAttribute("AreaRect_Top")->QueryIntValue(&area.AreaRect.top);
		pArea->FindAttribute("AreaRect_Right")->QueryIntValue(&area.AreaRect.width);
		pArea->FindAttribute("AreaRect_Bottom")->QueryIntValue(&area.AreaRect.height);

		area.AreaRect.width -= area.AreaRect.left;
		area.AreaRect.height -= area.AreaRect.top;
		//
		//	
#define FINDATTRIBUTE(p,x,v)		{int t=0;p->FindAttribute(x)->QueryIntValue(&t);v=t;}

		auto pSingleLineArea = pArea->FirstChildElement("SingleLineArea"); 
		if (pSingleLineArea)
		{
			pSingleLineArea->FindAttribute("InSpeed")->QueryIntValue(&area.singleLineArea.InSpeed);
			pSingleLineArea->FindAttribute("InStyle")->QueryIntValue(&area.singleLineArea.InStyle);
			pSingleLineArea->FindAttribute("OutStyle")->QueryIntValue(&area.singleLineArea.OutStyle);
			pSingleLineArea->FindAttribute("DelayTime")->QueryIntValue(&area.singleLineArea.DelayTime);
			auto pV = pSingleLineArea->GetText();
			std::string decoded = base64_decode(std::string(pV));
			parseFontColor_size_From_RTF(decoded,area.singleLineArea.FontColor,area.singleLineArea.FontSize);
		
			area.areaType=Area::SINGLELINEAREA ;
		}
		else
		{
			auto pDigitalClockArea = pArea->FirstChildElement("DigitalClockArea");
			if (pDigitalClockArea)
			{
				/*<DigitalClockArea ShowStr = ""
					ShowStrFont_FontName = "����" 
					ShowStrFont_FontSize = "12" 
					ShowStrFont_FontColor = "255" 
					ShowStrFont_FontBold = "0" 
					ShowStrFont_FontItalic = "0" 
					ShowStrFont_FontUnderLine = "0" 
					TimeLagType = "0" 
					HourNum = "0" 
					MiniteNum = "0" 
					DateFormat = "0" 					DateColor = "255" 
					WeekFormat = "0" 					WeekColor = "255" 
					TimeFormat = "2" TimeColor = "255" 
					IsShowYear = "0" IsShowWeek = "0" 
					IsShowMonth = "0" IsShowDay = "0" 
					IsShowHour = "0" IsShowMinite = "0" 
					IsShowSecond = "1" IsMutleLineShow = "0" 
					IsFlashColor = "0" FlashColorSpeed = "0" m_TransparentColor = "0" FlashColorIndex_0 = "1" FlashColorIndex_1 = "0" FlashColorIndex_2 = "0" FlashColorIndex_3 = "0" FlashColorIndex_4 = "0" FlashColorIndex_5 = "0" FlashColorIndex_6 = "0" FlashColorIndex_7 = "0" / >
/**/
				memset(&area.clockIfo, 0, sizeof(area.clockIfo));
				FINDATTRIBUTE(pDigitalClockArea, "DateColor", area.clockIfo.DateColor);
				FINDATTRIBUTE(pDigitalClockArea, "DateFormat", area.clockIfo.DateFormat);
				FINDATTRIBUTE(pDigitalClockArea, "WeekColor", area.clockIfo.WeekColor);
				FINDATTRIBUTE(pDigitalClockArea, "WeekFormat", area.clockIfo.WeekFormat);
				FINDATTRIBUTE(pDigitalClockArea, "TimeColor", area.clockIfo.TimeColor);
				FINDATTRIBUTE(pDigitalClockArea, "TimeFormat", area.clockIfo.TimeFormat);
							  
				FINDATTRIBUTE(pDigitalClockArea, "IsShowYear", area.clockIfo.IsShowYear);
				FINDATTRIBUTE(pDigitalClockArea, "IsShowMonth", area.clockIfo.IsShowMonth);
				FINDATTRIBUTE(pDigitalClockArea, "IsShowDay", area.clockIfo.IsShowDay);
				FINDATTRIBUTE(pDigitalClockArea, "IsShowHour", area.clockIfo.IsShowHour);
				FINDATTRIBUTE(pDigitalClockArea, "IsShowMinite", area.clockIfo.IsShowMinute);
				FINDATTRIBUTE(pDigitalClockArea, "IsShowSecond", area.clockIfo.IsShowSecond);
				FINDATTRIBUTE(pDigitalClockArea, "IsMutleLineShow", area.clockIfo.IsMutleLineShow);
				
				area.areaType = Area::TIME_AREA;
			}
			else
			{
				auto pNeiMaArea = pArea->FirstChildElement("NeiMaArea");
				if (pNeiMaArea)
				{
					FINDATTRIBUTE(pNeiMaArea, "InStyle", area.neiMaArea.InStyle );
					FINDATTRIBUTE(pNeiMaArea, "OutStyle", area.neiMaArea.OutStyle );
					FINDATTRIBUTE(pNeiMaArea, "PlaySpeed", area.neiMaArea.PlaySpeed );
					FINDATTRIBUTE(pNeiMaArea, "DelayTime", area.neiMaArea.DelayTime );
					FINDATTRIBUTE(pNeiMaArea, "FontSize", area.neiMaArea.FontSize );
					FINDATTRIBUTE(pNeiMaArea, "FontColor", area.neiMaArea.FontColor );

					area.areaType = Area::NEIMA_AREA;
				}
			}
		}
		program.areas.push_back(area);
	}
	led.programs.push_back(program);
	leds.push_back(led);

}
