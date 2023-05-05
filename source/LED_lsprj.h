
#include "tinyxml2.h"

class LED_lsprj {
public:
	void loadFile(const char* filePath);

	bool loadMem(const char* pText);
	struct Area {
		int AreaNo = 1;
		int AreaRect_Left = 0;
		int AreaRect_Top = 0;
		int AreaRect_Right = 40;
		int AreaRect_Bottom = 40;
	};
	struct Program {
		std::vector< Area> areas;
	};
	struct LED {
		std::vector<Program> programs;
		int   ModuleWidth = 0;
		int 	ModuleHeight = 0;
		int WidthModuleNum = 0;
		int 	HeightModuleNum = 0;
		int LedWidth = 0;
		int 	LedHeight = 0;

	};

	std::vector<LED> leds;

private:
	void parse(tinyxml2::XMLDocument& doc);

};