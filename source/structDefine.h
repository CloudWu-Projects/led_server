#pragma once
#include "json.hpp"
struct TaskData {
	int F_id;
	std::string F_message;
	int F_color=0xFF;
	int F_size=14;
	int top =-1;
	int left=-1;
	int width=-1;
	int height=-1;
};
struct LedInfo {
	int LedType;
	int LedWidth;
	int LedHeight;
	int LedColor;
	int LedGray;
};
struct LedTask{
	std::string park_id;
	std::string LED_id;
	std::string background;
	LedInfo led_info;
	
	std::vector<TaskData> data;

};


inline void from_json(const nlohmann::json& j, TaskData& p) {
	j.at("F_id").get_to(p.F_id);
	j.at("F_message").get_to(p.F_message);
		
	if(j.contains("F_color"))
		j.at("F_color").get_to(p.F_color);
	if(j.contains("F_size"))
		j.at("F_size").get_to(p.F_size);
		
	if(j.contains("top"))
		j.at("top").get_to(p.top);
	if(j.contains("left"))
		j.at("left").get_to(p.left);
	if(j.contains("width"))
		j.at("width").get_to(p.width);
	if(j.contains("height"))
		j.at("height").get_to(p.height);
}

inline void from_json(const nlohmann::json& j, LedInfo& p) {
	j.at("LedType").get_to(p.LedType);
	j.at("LedWidth").get_to(p.LedWidth);
	j.at("LedHeight").get_to(p.LedHeight);
	j.at("LedColor").get_to(p.LedColor);
	j.at("LedGray").get_to(p.LedGray);
}

inline void from_json(const nlohmann::json& j, LedTask& p) {
	j.at("park_id").get_to(p.park_id);
	j.at("LED_id").get_to(p.LED_id);
	j.at("background").get_to(p.background);
	j.at("data").get_to(p.data);
	j.at("led_info").get_to(p.led_info);

}
