#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include "./lib/ledplayer7.h"
#include <string>
#include "config.h"
// 获取配置文件参数
#include <set>
#include "demofunction_1.h"
std::set<std::string> ledList;
void demofunction_2(char *pIp, int ledType, int ledWidth, int ledHeight, int colorType, int grayLevel); // 一个节目下只有一个连接左移的单行文本区

int LedServerCallback(int Msg, int wParam, void *lParam); // 广域网开发回调函数，可实时知道哪个控制卡上下线

int main(int avgc, char *argv[])
{
    if (avgc == 1)
    {
        printf("参数1 设置屏参（注意：只需根据屏的宽高点数的颜色设置一次，发送节目时无需设置）\n");
        printf("参数2 一个节目下只有一个连接左移的单行文\n");
        printf("参数3 一个节目下只有一个多行文本区\n");
        printf("参数4 一个节目下只有一个图片区(表格的显示通过自绘图片并通过此方式添加发送)\n");
        printf("参数5 一个节目下有一个连续上移的单行文本区和一个图片区(多个区域都通过此方法添加)\n");
        printf("参数6 两个节目下各有一个单行文本区和一个数字时钟区(多节目通过此方法添加)\n");
    }
    else
    {

        char ip[16];
        GetProfileString("./lv.conf", "Public", "ip", ip);
        char strLEDType[16];
        GetProfileString("./lv.conf", "Public", "LEDType", strLEDType);
        char strLedWidth[16];
        GetProfileString("./lv.conf", "Public", "LedWidth", strLedWidth);
        char strLedHeight[16];
        GetProfileString("./lv.conf", "Public", "LedHeight", strLedHeight);
        char strColorType[16];
        GetProfileString("./lv.conf", "Public", "ColorType", strColorType);
        char strGrayLevel[16];
        GetProfileString("./lv.conf", "Public", "GrayLevel", strGrayLevel);
        int nLEDType = atol(strLEDType);
        int nLedWidth = atol(strLedWidth);
        int nLedHeight = atol(strLedHeight);
        int nColorType = atol(strColorType);
        int nGrayLevel = atol(strGrayLevel);

        if (*argv[1] == '2')
            demofunction_2(ip, nLEDType, nLedWidth, nLedHeight, nColorType, nGrayLevel);
        else if (*argv[1] == '3')
            demofunction_3(ip, nLEDType, nLedWidth, nLedHeight, nColorType, nGrayLevel);
        else if (*argv[1] == '4')
            demofunction_4(ip, nLEDType, nLedWidth, nLedHeight, nColorType, nGrayLevel);
        else if (*argv[1] == '5')
            demofunction_5(ip, nLEDType, nLedWidth, nLedHeight, nColorType, nGrayLevel);
        else if (*argv[1] == '6')
            demofunction_6(ip, nLEDType, nLedWidth, nLedHeight, nColorType, nGrayLevel);
        else
        {
            // 广域网开发,才打开以下两个api，C4M C2M才支持
            // 打开服务
            LV_InitServer(10008);                         // 启动服务，维护控制卡心跳
            LV_RegisterServerCallback(LedServerCallback); // 注册回调，回调消息告知上下线控制卡的信息，只有上线的控制卡，才能成功发送节目
            // 断开服务
            // LV_LedShudownServer();
            while(1)
            {
                getchar();
                for(auto ip:ledList)
                    demofunction_2(ip.data(), nLEDType, nLedWidth, nLedHeight, nColorType, nGrayLevel);
            }
        }
    }
    return 0;
}

// 一个节目下只有一个连接左移的单行文
void demofunction_2(char *pIp, int ledType, int ledWidth, int ledHeight, int colorType, int grayLevel)
{
    printf("一个节目下只有一个连接左移的单行文\n");
    int nResult;
    COMMUNICATIONINFO CommunicationInfo;
    memset(&CommunicationInfo, 0, sizeof(COMMUNICATIONINFO));
    CommunicationInfo.LEDType = ledType;
    // TCP通讯********************************************************************************
    CommunicationInfo.SendType = 0;       // 设为固定IP通讯模式，即TCP通讯
    strcpy(CommunicationInfo.IpStr, pIp); // 给IpStr赋值LED控制卡的IP
    CommunicationInfo.LedNumber = 1;
    // 广播通讯********************************************************************************
    // CommunicationInfo.SendType=1;//设为单机直连，即广播通讯无需设LED控制器的IP地址
    // 广域网通讯
     CommunicationInfo.SendType=4;//设为广域网通讯模式
     strcpy(CommunicationInfo.NetworkIdStr,pIp);//指定唯一网络ID

    /***第一步创建节目句柄***开始***/
    HPROGRAM hProgram;                                                           // 节目句柄
    hProgram = LV_CreateProgramEx(ledWidth, ledHeight, colorType, grayLevel, 0); // 根据传的参数创建节目句柄，注意此处屏宽高及颜色参数必需与设置屏参的屏宽高及颜色一致，否则发送时会提示错误
    /***第一步创建节目句柄***结束***/

    /***第二步添加一个节目***开始***/
    int nProgramNo = 0;
    nResult = LV_AddProgram(hProgram, nProgramNo, 0, 1);
    if (nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult, 256, ErrStr);
        printf("%s\n", ErrStr);
        return;
    }
    /***第二步添加一个节目***结束***/

    /***第三步快速添加单行文本区域（默认连续左移）到节目号为nProgramNo***开始***/
    AREARECT AreaRect; // 区域坐标属性结构体变量
    AreaRect.left = 0;
    AreaRect.top = 0;
    AreaRect.width = ledWidth;
    AreaRect.height = ledHeight;

    FONTPROP FontProp; // 文字属性s
    memset(&FontProp, 0, sizeof(FONTPROP));
    strcpy(FontProp.FontPath, "./font/simsun.ttc");
    FontProp.FontSize = 14;
    FontProp.FontColor = COLOR_RED;

    nResult = LV_QuickAddSingleLineTextArea(hProgram, nProgramNo, 1, &AreaRect, ADDTYPE_STRING, "上海灵信视觉技术股份有限公司", &FontProp, 20); // 快速通过字符添加一个单行文本区域
    // nResult=LV_QuickAddSingleLineTextArea(hProgram,nProgramNo,1,&AreaRect,ADDTYPE_FILE,"test.txt",&FontProp,4);//快速通过txt文件添加一个单行文本区域

    if (nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult, 256, ErrStr);
        printf("%s\n", ErrStr);
        return;
    }
    /***第三步快速添加单行文本区域（默认连续左移）***结束***/

    /***第四步发送节目以及删除节目内存对象***开始***/
    nResult = LV_Send(&CommunicationInfo, hProgram); // 发送，见函数声明注示
    LV_DeleteProgram(hProgram);                      // 删除节目内存对象，详见函数声明注示
    if (nResult)
    {
        char ErrStr[256];
        LV_GetError(nResult, 256, ErrStr);
        printf("%s\n", ErrStr);
        return;
    }
    else
    {
        printf("发送成功\n");
    }
    /***第四步发送节目以及删除节目内存对象***结束***/
}

// 回调函数，可实时知道哪个控制卡上下线 //只支持C2M  C4M
int LedServerCallback(int Msg, int wParam, void *lParam)
{
    switch (Msg)
    {
    case LV_MSG_CARD_ONLINE:
    {
        LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;
        printf("LV_MSG_CARD_ONLINE\n");
        printf("pCardInfo->port:%d\n", pCardInfo->port);
        printf("pCardInfo->ipStr:%s\n", pCardInfo->ipStr);
        printf("pCardInfo->networkIdStr:%s\n", pCardInfo->networkIdStr);
        ledList.emplace(pCardInfo->networkIdStr);
    }
    break;
    case LV_MSG_CARD_OFFLINE:
    {
        LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;
        printf("LV_MSG_CARD_OFFLINE\n");
        printf("pCardInfo->port:%d\n", pCardInfo->port);
        printf("pCardInfo->ipStr:%s\n", pCardInfo->ipStr);
        printf("pCardInfo->networkIdStr:%s\n", pCardInfo->networkIdStr);
        ledList.erase(pCardInfo->networkIdStr);
    }
    break;
    }
    return 0;
}
