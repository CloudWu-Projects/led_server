#pragma once
#include "led.h"
#include <vector>
struct Area
{
    int AreaNo = 1;
    AREARECT AreaRect;
    
	struct SingleLineArea {
		int InSpeed = 0;
		int InStyle = 0;
		int OutStyle = 0;
		int DelayTime = 0;
		int FontColor = 0; // BBGGRR
		int BackColor = 0; //
        int FontSize = 0;
	};
    SingleLineArea singleLineArea;
    struct NeiMaArea {
        int InStyle = 0;
		int OutStyle = 0;
		int  PlaySpeed = 3;
		int  DelayTime = 3;
		int  FontSize = 0;
        int  FontColor = 255;
	};
    NeiMaArea neiMaArea;
    enum AreaType
    {
        SINGLELINEAREA=0,
        TIME_AREA,
        NEIMA_AREA
    };
    DIGITALCLOCKAREAINFO clockIfo ;
    AreaType areaType = SINGLELINEAREA;
    ~Area()
    {
        

    }
};
struct Program
{
    std::vector<Area> areas;
};
struct LED
{
    std::vector<Program> programs;
    int ModuleWidth = 0;
    int ModuleHeight = 0;
    int WidthModuleNum = 0;
    int HeightModuleNum = 0;
    int LedWidth = 0;
    int LedHeight = 0;
    int LedType = 0;
    int LedColor = 0;
    int LedGray = 0;
};
