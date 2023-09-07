#pragma  once
#include <string>
#include "hv/json.hpp"

namespace ns {
	struct  JsonArea {
		std::string name;
		std::string value;

		int fontSize = -1;
		int fontColor = -1;
	};

	struct  LanfengLED {
		std::vector<std::string> ledids;
		int isIP = 0;
		std::string pgmfilepath;
		std::string park_id;
		std::vector< JsonArea> areas;
		int fontSize = -1;
		int fontColor = -1;
		LanfengLED() {
			ledids.clear();
			park_id = "";
		}
		bool  find(std::string aeraName, JsonArea&area) {
			for (auto & item : areas)
			{
				if (item.name == aeraName)
				{
					area = item;
					return true;
				}
			}
			return	false;
		}
	};

	// 为JsonArea结构体实现from_json函数
	inline void from_json(const nlohmann::json& j, JsonArea& area) {
		j.at("name").get_to(area.name);
		j.at("value").get_to(area.value);
		area.name = UTF8_ASCI::UTF_82ASCII(area.name);
		area.value = UTF8_ASCI::UTF_82ASCII(area.value);
		if(j.contains("fontSize"))
			j.at("fontSize").get_to(area.fontSize);
		if (j.contains("fontColor"))
			j.at("fontColor").get_to(area.fontColor);
	}

	// 为LanfengLED结构体实现from_json函数
	inline void from_json(const nlohmann::json& j, LanfengLED& person) {
		j.at("ledids").get_to(person.ledids);
		j.at("pgmfilepath").get_to(person.pgmfilepath);
		person.pgmfilepath = UTF8_ASCI::UTF_82ASCII(person.pgmfilepath);
		j.at("park_id").get_to(person.park_id);
		j.at("isip").get_to(person.isIP);
		j.at("fontSize").get_to(person.fontSize);
		j.at("fontColor").get_to(person.fontColor);
		j.at("areas").get_to(person.areas);
	}

}