#pragma once
#include <vector>
struct Area
{
    int AreaNo = 1;
    int AreaRect_Left = 0;
    int AreaRect_Top = 0;
    int AreaRect_Right = 40;
    int AreaRect_Bottom = 40;
    int InSpeed = 0;
    int InStyle = 0;
    int OutStyle = 0;
    int DelayTime = 0;
    int FontColor = 0; // BBGGRR
    int BackColor = 0; //
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
