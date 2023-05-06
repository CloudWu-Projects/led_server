#include "LED_lsprj.h"
#include "logLib.h"
#include "tinyxml2.h"
#include "cpp-base64/base64.h"
bool LED_lsprj::loadFile(const char*filePath) {
	tinyxml2::XMLDocument doc;
	if (auto error = doc.LoadFile(filePath);
		error != tinyxml2::XMLError::XML_SUCCESS)
	{
		SPDLOG_ERROR("LoadFile xml failed. {}", error);
		return false;
	}
	parse(&doc);
	return true;
}

bool LED_lsprj::loadMem(const char* pText) {

	tinyxml2::XMLDocument doc;
	if (auto error = doc.Parse(pText);
		error != tinyxml2::XMLError::XML_SUCCESS)
	{
		SPDLOG_ERROR("loadMem xml failed. {}", error);
		return false;
	}
	parse(&doc);
	return true;
}

int parseFontColor_From_RTF(std::string &RTFtext)
{
	/*
	TODO: RTFtext is
			{\rtf1\fbidis\ansi\ansicpg936\deff0\nouicompat\rtldoc{\fonttbl{\f0\fswiss\fprq2\fcharset134 \'cb\'ce\'cc\'e5;}}
{\colortbl ;\red0\green255\blue0;\red255\green0\blue0;}
{\*\generator Riched20 10.0.22621}\viewkind4\uc1
\pard\ltrpar\cf1\f0\fs32\lang2052\'d7\'d6\'c4\'bb1\cf2\fs24\par
}
	parser RTFtext to get color
			*/

	size_t redStart = RTFtext.find("\\red");
	size_t greenStart = RTFtext.find("\\green");
	size_t blueStart = RTFtext.find("\\blue");
	if (redStart == std::string::npos || greenStart == std::string::npos || blueStart == std::string::npos)
	{
		// Malformed color table
		return -1;
	}
	redStart += 4;
	greenStart += 6;
	blueStart += 5;
	int red = std::stoi(RTFtext.substr(redStart, greenStart - redStart - 1));
	int green = std::stoi(RTFtext.substr(greenStart, blueStart - greenStart - 1));
	int blue = std::stoi(RTFtext.substr(blueStart));
	int color = (blue << 16) | (green << 8) | red;
	//BBGGRR
	return color;
}
void LED_lsprj::parse(tinyxml2::XMLDocument* doc)
{

	clear();
	std::lock_guard<std::mutex> lock(leds_mutex);
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
		pArea->FindAttribute("AreaRect_Left")->QueryIntValue(&area.AreaRect_Left);
		pArea->FindAttribute("AreaRect_Top")->QueryIntValue(&area.AreaRect_Top);
		pArea->FindAttribute("AreaRect_Right")->QueryIntValue(&area.AreaRect_Right);
		pArea->FindAttribute("AreaRect_Bottom")->QueryIntValue(&area.AreaRect_Bottom);

		auto pSingleLineArea = pArea->FirstChildElement("SingleLineArea");
		if (pSingleLineArea)
		{
			pSingleLineArea->FindAttribute("InSpeed")->QueryIntValue(&area.InSpeed);
			pSingleLineArea->FindAttribute("InStyle")->QueryIntValue(&area.InStyle);
			pSingleLineArea->FindAttribute("OutStyle")->QueryIntValue(&area.OutStyle);
			pSingleLineArea->FindAttribute("DelayTime")->QueryIntValue(&area.DelayTime);
			auto pV = pSingleLineArea->GetText();
			std::string decoded = base64_decode(pV);
			
			area.FontColor = parseFontColor_From_RTF(decoded);
		

		}
		program.areas.push_back(area);
	}
	led.programs.push_back(program);
	leds.push_back(led);

}

void LED_lsprj::clear() {

	std::lock_guard<std::mutex> lock(leds_mutex);
	for (auto led : leds)
	{
		for (auto p : led.programs)
		{
			p.areas.clear();
		}
		led.programs.clear();
	}
	leds.clear();
}
