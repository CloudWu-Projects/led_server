#pragma once
#include <vector>

#include "ledPgm.h"
namespace tinyxml2 { class XMLDocument; };
class LED_lsprj {
public:
	bool loadFile(const char* filePath,std::vector<LED>&leds);

	bool loadMem(const char* pText,std::vector<LED>&leds);
	
private:
	void parse(tinyxml2::XMLDocument* doc,std::vector<LED>&leds);
	void clear();



};