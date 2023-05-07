#pragma once
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include "./lib/ledplayer7.h"

#include "config.h"
//设置屏参（注意：只需根据屏的宽高点数的颜色设置一次，发送节目时无需设置）
void demofunction_1(char *pIp,int ledType,int ledWidth,int ledHeight,int colorType,int grayLevel)
{
    printf("设置屏参（注意：只需根据屏的宽高点数的颜色设置一次，发送节目时无需设置）\n");
    int nResult;
    COMMUNICATIONINFO CommunicationInfo;
    memset(&CommunicationInfo,0,sizeof(COMMUNICATIONINFO));
    CommunicationInfo.LEDType=ledType;
    //TCP通讯********************************************************************************
    CommunicationInfo.SendType=0;//设为固定IP通讯模式，即TCP通讯
    strcpy(CommunicationInfo.IpStr,pIp);//给IpStr赋值LED控制卡的IP
    CommunicationInfo.LedNumber=1;
    //广播通讯********************************************************************************
    //CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
    //广域网通讯
    //CommunicationInfo.SendType=4;//设为广域网通讯模式
    //strcpy(CommunicationInfo.NetworkIdStr,"960401949002732");//指定唯一网络ID

    nResult=LV_SetBasicInfoEx(&CommunicationInfo,colorType,grayLevel,ledWidth,ledHeight);//设置屏参，具体函数参数说明查看文档

    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
    else
    {
        printf("设置成功\n");
    }
}
//一个节目下只有一个多行文本区
void demofunction_3(char *pIp,int ledType,int ledWidth,int ledHeight,int colorType,int grayLevel)
{
    printf("一个节目下只有一个多行文本区\n");
    int nResult;
    COMMUNICATIONINFO CommunicationInfo;
    memset(&CommunicationInfo,0,sizeof(COMMUNICATIONINFO));
    CommunicationInfo.LEDType=ledType;
    //TCP通讯********************************************************************************
    CommunicationInfo.SendType=0;//设为固定IP通讯模式，即TCP通讯
    strcpy(CommunicationInfo.IpStr,pIp);//给IpStr赋值LED控制卡的IP
    CommunicationInfo.LedNumber=1;
    //广播通讯********************************************************************************
    //CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
    //广域网通讯
    //CommunicationInfo.SendType=4;//设为广域网通讯模式
    //strcpy(CommunicationInfo.NetworkIdStr,"960401949002732");//指定唯一网络ID

/***第一步创建节目句柄***开始***/
    HPROGRAM hProgram;//节目句柄
    hProgram=LV_CreateProgramEx(ledWidth,ledHeight,colorType,grayLevel,0);//根据传的参数创建节目句柄，注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
/***第一步创建节目句柄***结束***/

/***第二步添加一个节目***开始***/
    int nProgramNo = 0;
    nResult=LV_AddProgram(hProgram,nProgramNo,0,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第二步添加一个节目***结束***/

/***第三步添加一个多行文本区域到节目号为nProgramNo***开始***/
    AREARECT AreaRect;//区域坐标属性结构体变量
    AreaRect.left=0;
    AreaRect.top=0;
    AreaRect.width=ledWidth;
    AreaRect.height=ledHeight;

    nResult=LV_AddImageTextArea(hProgram,nProgramNo,1,&AreaRect,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }

    FONTPROP FontProp;//文字属性
    memset(&FontProp,0,sizeof(FONTPROP));
    strcpy(FontProp.FontPath,"./font/simsun.ttc");
    FontProp.FontSize=14;
    FontProp.FontColor=COLOR_RED;

    PLAYPROP PlayProp;//显示属性
    PlayProp.DelayTime=3;
    PlayProp.InStyle=0;
    PlayProp.OutStyle=0;
    PlayProp.Speed=1;

    //可以添加多个子项到图文区，如下添加可以选一个或多个添加
    //nResult=LV_AddStaticTextToImageTextArea(hProgram,nProgramNo,1,ADDTYPE_STRING,"上海灵信视觉技术股份有限公司",&FontProp,3,0,FALSE);
    nResult=LV_AddMultiLineTextToImageTextArea(hProgram,nProgramNo,1,ADDTYPE_STRING,"上海灵信视觉技术股份有限公司",&FontProp,&PlayProp,0,FALSE);//通过字符串添加一个多行文本到图文区，参数说明见声明注示
    //nResult=LV_AddMultiLineTextToImageTextArea(hProgram,nProgramNo,1,ADDTYPE_FILE,"test.txt",&FontProp,&PlayProp,0,FALSE);//通过txt文件添加一个多行文本到图文区，参数说明见声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第三步添加一个多行文本区域***结束***/

/***第四步发送节目以及删除节目内存对象***开始***/
    nResult=LV_Send(&CommunicationInfo,hProgram);//发送，见函数声明注示
    LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
    else
    {
        printf("发送成功\n");
    }
/***第四步发送节目以及删除节目内存对象***结束***/
}


//一个节目下只有一个图片区(表格的显示通过自绘图片并通过此方式添加发送)
void demofunction_4(char *pIp,int ledType,int ledWidth,int ledHeight,int colorType,int grayLevel)
{
    printf("一个节目下只有一个图片区(表格的显示通过自绘图片并通过此方式添加发送)\n");
    int nResult;
    COMMUNICATIONINFO CommunicationInfo;
    memset(&CommunicationInfo,0,sizeof(COMMUNICATIONINFO));
    CommunicationInfo.LEDType=ledType;
    //TCP通讯********************************************************************************
    CommunicationInfo.SendType=0;//设为固定IP通讯模式，即TCP通讯
    strcpy(CommunicationInfo.IpStr,pIp);//给IpStr赋值LED控制卡的IP
    CommunicationInfo.LedNumber=1;
    //广播通讯********************************************************************************
    //CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
    //广域网通讯
    //CommunicationInfo.SendType=4;//设为广域网通讯模式
    //strcpy(CommunicationInfo.NetworkIdStr,"960401949002732");//指定唯一网络ID

/***第一步创建节目句柄***开始***/
    HPROGRAM hProgram;//节目句柄
    hProgram=LV_CreateProgramEx(ledWidth,ledHeight,colorType,grayLevel,0);//根据传的参数创建节目句柄，注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误

/***第一步创建节目句柄***结束***/

/***第二步添加一个节目***开始***/
    int nProgramNo = 0;
    nResult=LV_AddProgram(hProgram,nProgramNo,0,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第二步添加一个节目***结束***/

/***第三步添加一个图片区域到节目号为nProgramNo***开始***/
    AREARECT AreaRect;//区域坐标属性结构体变量
    AreaRect.left=0;
    AreaRect.top=0;
    AreaRect.width=ledWidth;
    AreaRect.height=ledHeight;

    nResult=LV_AddImageTextArea(hProgram,nProgramNo,1,&AreaRect,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }

    PLAYPROP PlayProp;//显示属性
    PlayProp.DelayTime=3;
    PlayProp.InStyle=0;
    PlayProp.OutStyle=0;
    PlayProp.Speed=1;

    //可以添加多个子项到图文区，如下添加可以选一个或多个添加
    nResult=LV_AddFileToImageTextArea(hProgram,nProgramNo,1,"test.bmp",&PlayProp);
    nResult=LV_AddFileToImageTextArea(hProgram,nProgramNo,1,"test.jpg",&PlayProp);

    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第三步添加一个图片区域***结束***/

/***第四步发送节目以及删除节目内存对象***开始***/
    nResult=LV_Send(&CommunicationInfo,hProgram);//发送，见函数声明注示
    LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
    else
    {
        printf("发送成功\n");
    }
/***第四步发送节目以及删除节目内存对象***结束***/
}


//一个节目下有一个连续上移的单行文本区和一个图片区(多个区域都通过此方法添加)
void demofunction_5(char *pIp,int ledType,int ledWidth,int ledHeight,int colorType,int grayLevel)
{
    printf("一个节目下有一个连续上移的单行文本区和一个图片区(多个区域都通过此方法添加)\n");
    int nResult;
    COMMUNICATIONINFO CommunicationInfo;
    memset(&CommunicationInfo,0,sizeof(COMMUNICATIONINFO));
    CommunicationInfo.LEDType=ledType;
    //TCP通讯********************************************************************************
    CommunicationInfo.SendType=0;//设为固定IP通讯模式，即TCP通讯
    strcpy(CommunicationInfo.IpStr,pIp);//给IpStr赋值LED控制卡的IP
    CommunicationInfo.LedNumber=1;
    //广播通讯********************************************************************************
    //CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
    //广域网通讯
    //CommunicationInfo.SendType=4;//设为广域网通讯模式
    //strcpy(CommunicationInfo.NetworkIdStr,"960401949002732");//指定唯一网络ID

/***第一步创建节目句柄***开始***/
    HPROGRAM hProgram;//节目句柄
    hProgram=LV_CreateProgramEx(ledWidth,ledHeight,colorType,grayLevel,0);//根据传的参数创建节目句柄，注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
/***第一步创建节目句柄***结束***/

/***第二步添加一个节目***开始***/
    int nProgramNo = 0;
    nResult=LV_AddProgram(hProgram,nProgramNo,0,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第二步创建节目句柄***结束***/

/***第三步添加一个单行文本区和一个图片区到节目号为nProgramNo***开始***/
	
    /*********第一个区域*********/	
    AREARECT AreaRect;//区域坐标属性结构体变量
    AreaRect.left=0;
    AreaRect.top=0;
    AreaRect.width=ledWidth;
    AreaRect.height=ledHeight/2;

    nResult=LV_AddImageTextArea(hProgram,nProgramNo,1,&AreaRect,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }

    FONTPROP FontProp;//文字属性
    memset(&FontProp,0,sizeof(FONTPROP));
    strcpy(FontProp.FontPath,"./font/simsun.ttc");
    FontProp.FontSize=12;
    FontProp.FontColor=COLOR_RED;

    PLAYPROP PlayProp;//显示属性
    PlayProp.DelayTime=0;
    PlayProp.InStyle=8;
    PlayProp.OutStyle=0;
    PlayProp.Speed=20;

    //可以添加多个子项到图文区，如下添加可以选一个或多个添加
    nResult=LV_AddSingleLineTextToImageTextArea(hProgram,nProgramNo,1,ADDTYPE_STRING,"上海灵信视觉技术股份有限公司",&FontProp,&PlayProp);
    //nResult=LV_AddSingleLineTextToImageTextArea(hProgram,nProgramNo,1,ADDTYPE_FILE,"test.txt",&FontProp,&PlayProp);//通过txt文件添加一个单行文本到图文区，参数说明见声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }


    /*********第二个区域*********/
    //区域坐标属性结构体变量
    AreaRect.left=0;
    AreaRect.top=ledHeight/2;
    AreaRect.width=ledWidth;
    AreaRect.height=ledHeight/2;

    nResult=LV_AddImageTextArea(hProgram,nProgramNo,2,&AreaRect,1);
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }

    PlayProp.DelayTime=3;
    PlayProp.InStyle=0;
    PlayProp.OutStyle=0;
    PlayProp.Speed=1;

    //可以添加多个子项到图文区，如下添加可以选一个或多个添加
    nResult=LV_AddFileToImageTextArea(hProgram,nProgramNo,2,"test.bmp",&PlayProp);

    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第四步添加图片文件到图文区***结束***/

/***第四步发送节目以及删除节目内存对象***开始***/
    nResult=LV_Send(&CommunicationInfo,hProgram);//发送，见函数声明注示
    LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
    else
    {
        printf("发送成功\n");
    }
/***第四步发送节目以及删除节目内存对象***结束***/
}


//两个节目下各有一个单行文本区和一个数字时钟区(多节目通过此方法添加)
void demofunction_6(char *pIp,int ledType,int ledWidth,int ledHeight,int colorType,int grayLevel)
{
    printf("两个节目下各有一个单行文本区和一个数字时钟区(多节目通过此方法添加)\n");
    int nResult;
    COMMUNICATIONINFO CommunicationInfo;
    memset(&CommunicationInfo,0,sizeof(COMMUNICATIONINFO));
    CommunicationInfo.LEDType=ledType;
    //TCP通讯********************************************************************************
    CommunicationInfo.SendType=0;//设为固定IP通讯模式，即TCP通讯
    strcpy(CommunicationInfo.IpStr,pIp);//给IpStr赋值LED控制卡的IP
    CommunicationInfo.LedNumber=1;
    //广播通讯********************************************************************************
    //CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
    //广域网通讯
    //CommunicationInfo.SendType=4;//设为广域网通讯模式
    //strcpy(CommunicationInfo.NetworkIdStr,"960401949002732");//指定唯一网络IDs

/***第一步创建节目句柄***开始***/
    HPROGRAM hProgram;//节目句柄
    hProgram=LV_CreateProgramEx(ledWidth,ledHeight,colorType,grayLevel,0);//根据传的参数创建节目句柄，注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
/***第一步创建节目句柄***结束***/

/***第二步添加第一个单行文本的节目***开始***/
    int nProgramNo = 0;//第一个节目为0
    nResult=LV_AddProgram(hProgram,nProgramNo,0,1);//添加一个节目，参数说明见函数声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);//见函数声明注示
        printf("%s\n",ErrStr);
        return;
    }

    //添加一个单行文本
    AREARECT AreaRect;//区域坐标属性结构体变量
    AreaRect.left=0;
    AreaRect.top=0;
    AreaRect.width=ledWidth;
    AreaRect.height=ledHeight;

    FONTPROP FontProp;//文字属性
    memset(&FontProp,0,sizeof(FONTPROP));
    strcpy(FontProp.FontPath,"./font/simsun.ttc");
    FontProp.FontSize=12;
    FontProp.FontColor=0xff;

    nResult=LV_QuickAddSingleLineTextArea(hProgram,nProgramNo,1,&AreaRect,ADDTYPE_STRING,"上海灵信视觉技术股份有限公司",&FontProp,20);//快速通过字符添加一个单行文本区域，函数见函数声明注示
/***第二步添加第一个单行文本的节目***结束***/


/***第三步添加第二个数字时钟的节目***开始***/
    nProgramNo = 1;//第二个节目为1，以此类推
    nResult=LV_AddProgram(hProgram,nProgramNo,0,1);//添加一个节目，参数说明见函数声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }

    //添加一个数字时钟区域
    AreaRect.left=0;
    AreaRect.top=0;
    AreaRect.width=ledWidth;
    AreaRect.height=ledHeight;

    DIGITALCLOCKAREAINFO clockIfo;
    memset(&clockIfo,0,sizeof(DIGITALCLOCKAREAINFO));
    clockIfo.DateColor = 0xff;
    clockIfo.WeekColor = 0xff;
    clockIfo.TimeColor = 0xff;
    clockIfo.IsShowHour = 1;
    clockIfo.IsShowMinute = 1;
    clockIfo.IsShowSecond = 1;
    clockIfo.IsShowYear = 1;
    clockIfo.IsShowMonth = 1;
    clockIfo.IsShowDay = 0;
    clockIfo.IsShowWeek = 1;
    clockIfo.IsMutleLineShow = 1;

    strcpy(clockIfo.ShowStrFont.FontPath,"./font/simsun.ttc");
    clockIfo.ShowStrFont.FontSize=10;
    clockIfo.ShowStrFont.FontColor=COLOR_RED;
    nResult=LV_AddDigitalClockArea(hProgram,nProgramNo,1,&AreaRect,&clockIfo);

    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
/***第三步添加第二个数字时钟的节目***结束***/

/***第四步发送节目以及删除节目内存对象***开始***/
    nResult=LV_Send(&CommunicationInfo,hProgram);//发送，见函数声明注示
    LV_DeleteProgram(hProgram);//删除节目内存对象，详见函数声明注示
    if(nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult,256,ErrStr);
        printf("%s\n",ErrStr);
        return;
    }
    else
    {
        printf("发送成功\n");
    }
/***第四步发送节目以及删除节目内存对象***结束***/
}
