#pragma  once
#include <string>
#include "hv/json.hpp"
class JsonArea {
public:
	std::string name;
	std::string value;

	int fontSize = -1;
	int fontColor = -1;
};
class  LanfengLED {
public:
	std::vector<std::string> ledids;
	bool isip=false;
	std::string pgmfilepath;
	std::string park_id;
	int store_status_1;
	int store_status_2;
	int store_status_3;
	int store_status_4;
	int store_status_5;
	int store_status_6;
	int store_status_7;
	int store_status_8;
	std::map<std::string, std::string> content;

	int fontSize=-1;
	int fontColor=-1;
	LanfengLED() {
		ledids.clear();
		park_id = "";
		store_status_1 = 0;
		store_status_2 = 0;
		store_status_3 = 0;
		store_status_4 = 0;
		store_status_5 = 0;
		store_status_6 = 0;
		store_status_7 = 0;
		store_status_8 = 0;
	}
};

namespace ns {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonArea, name, value, fontSize, fontColor);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LanfengLED, ledids, pgmfilepath, park_id, isip,
		store_status_1,
		store_status_2,
		store_status_3,
		store_status_4,
		store_status_5,
		store_status_6,
		store_status_7,
	    store_status_8,  fontSize, fontColor);

}