#pragma once
#include "ledPgm.h"
#include <string>
class LedPgm_json{

public:
	bool loadFile(const char* filePath, std::vector<LED>& leds);

	bool loadMem(const char* pText, std::vector<LED>& leds);

};