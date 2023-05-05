#include "LED_lsprj.h"
#include "logLib.h"
void LED_lsprj::loadFile(const char*filePath) {
	tinyxml2::XMLDocument doc;
	if (auto error = doc.LoadFile(filePath);
		error != tinyxml2::XMLError::XML_SUCCESS)
	{
		SPDLOG_ERROR("LoadFile xml failed. {}", error);
		return;
	}
	parse(doc);
}

bool LED_lsprj::loadMem(const char* pText) {

	tinyxml2::XMLDocument doc;
	if (auto error = doc.Parse(pText);
		error != tinyxml2::XMLError::XML_SUCCESS)
	{
		SPDLOG_ERROR("loadMem xml failed. {}", error);
		return false;
	}
	parse(doc);
	return true;
}

void LED_lsprj::parse(tinyxml2::XMLDocument& doc) {

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
	// Structure of the XML file:
	// - Element "PLAY"      the root Element, which is the
	//                       FirstChildElement of the Document
	// - - Element "TITLE"   child of the root PLAY Element
	// - - - Text            child of the TITLE Element

	// Navigate to the title, using the convenience function,
	// with a dangerous lack of error checking.
	auto pLed = doc.FirstChildElement("LEDS")->FirstChildElement("LED");
	LED led;
	pLed->FindAttribute("ModuleWidth")->QueryIntValue(&led.ModuleWidth);
	pLed->FindAttribute("ModuleHeight")->QueryIntValue(&led.ModuleHeight);
	pLed->FindAttribute("WidthModuleNum")->QueryIntValue(&led.WidthModuleNum);
	pLed->FindAttribute("HeightModuleNum")->QueryIntValue(&led.HeightModuleNum);
	pLed->FindAttribute("LedWidth")->QueryIntValue(&led.LedWidth);
	pLed->FindAttribute("LedHeight")->QueryIntValue(&led.LedHeight);

	Program program;
	for (auto pArea = pLed->FirstChildElement("Program")->FirstChildElement("Area"); pArea != nullptr; pArea = pArea->NextSiblingElement())
	{
		Area area;
		pArea->FindAttribute("AreaNo")->QueryIntValue(&area.AreaNo);
		pArea->FindAttribute("AreaRect_Left")->QueryIntValue(&area.AreaRect_Left);
		pArea->FindAttribute("AreaRect_Top")->QueryIntValue(&area.AreaRect_Top);
		pArea->FindAttribute("AreaRect_Right")->QueryIntValue(&area.AreaRect_Right);
		pArea->FindAttribute("AreaRect_Bottom")->QueryIntValue(&area.AreaRect_Bottom);
		program.areas.push_back(area);
	}
	led.programs.push_back(program);
	leds.push_back(led);

}
