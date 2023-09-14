#ifndef LEDPLAYER7_H
#define LEDPLAYER7_H

typedef void             *LPVOID;
typedef LPVOID				HPROGRAM;	//节目句柄类型
typedef unsigned long       DWORD;
typedef DWORD   COLORREF;
typedef char TCHAR;
typedef bool BOOL;
typedef unsigned int        UINT;
#define FALSE   0
#define TRUE    1
#define LPCTSTR const char *
#define LPTSTR  char *
#define MAX_PATH          260
#ifndef OUT
#define OUT
#endif

#define COLOR_RED			0xff		//红色
#define COLOR_GREEN			0xff00		//绿色
#define COLOR_YELLOW		0Xffff		//黄色


#define ADDTYPE_STRING		0			//添加类型为字串
#define ADDTYPE_FILE		1			//添加的类型为文件


#define OK					0			//函数返回成功

//******节目定时启用日期时间星期的标志宏***************************************************************************
#define ENABLE_DATE			0x01
#define ENABLE_TIME			0x02
#define ENABLE_WEEK			0x04
//*****************************************************************************************************************

//******节目定时星期里某天启用宏***********************************************************
#define WEEK_MON			0x01
#define WEEK_TUES			0x02
#define WEEK_WEN			0x04
#define WEEK_THUR			0x08
#define WEEK_FRI			0x10
#define WEEK_SAT			0x20
#define WEEK_SUN			0x40
//*****************************************************************************

//**通讯设置结构体*********************************************************
typedef struct COMMUNICATIONINFO
{
    int		LEDType;				//LED类型	0.6代T系A系XC系    1.6代E系     2.X1X2		3.C系
    int		SendType;				//通讯方式	0.为Tcp发送（又称固定IP通讯）    4.广域网通讯
    TCHAR	IpStr[16];				//LED屏的IP地址，只有通讯方式为0时才需赋值，其它通讯方式无需赋值
    int		Commport;				//串口号，只有通讯方式为2时才需赋值，其它通讯方式无需赋值
    int		Baud;					//波特率，只有通讯方式为2时才需赋值，其它通讯方式无需赋值,   0.9600   1.57600   2.115200  直接赋值 9600，19200，38400，57600，115200亦可
    int		LedNumber;				//LED的屏号，只有通讯方式为2时，且用485通讯时才需赋值，其它通讯方式无需赋值
    TCHAR	OutputDir[MAX_PATH];	//磁盘保存的目录，只有通讯方式为3时才需赋值，其它通讯方式无需赋值
    TCHAR	NetworkIdStr[19];	//网络ID,只有通讯方式为4时才需赋值，其它通讯方式无需赋值
}*LPCOMMUNICATIONINFO;
//***********************************************************************

//**区域坐标结构体*********************************************************
typedef struct AREARECT
{
    int			left;	//区域左上角横坐标
    int			top;	//区域左上角纵坐标
    int			width;	//区域的宽度
    int			height;	//区域的高度
}*LPAREARECT;
//****************************************************************************

//***字体属性结构对**********************************************************
struct FONTPROP
{
    char		FontPath[MAX_PATH];		//字体路径
    int			FontSize;			//字号(单位像素)
    COLORREF	FontColor;			//字体颜色
    BOOL		FontBold;			//是否加粗
    BOOL		FontItalic;			//是否斜体
    BOOL		FontUnderLine;		//时否下划线
};
//****************************************************************************

//YYYY年MM月DD日;YY年MM月DD日;DD/MM/YYYY;YYYY/MM/DD;YYYY-MM-DD;YYYY.MM.DD;MM.DD.YYYY;DD.MM.YYYY;
typedef struct DIGITALCLOCKAREAINFO
{
    TCHAR ShowStr[128];				//自定义显示字符串
    FONTPROP ShowStrFont;			//自定义显示字符串字体
    int TimeLagType;				//滞后类型 0为超前，1为滞后
    int HourNum;					//小时数
    int MiniteNum;					//分钟数

    int DateFormat;
    COLORREF DateColor;
    int WeekFormat;
    COLORREF WeekColor;
    int TimeFormat;
    COLORREF TimeColor;

    BOOL IsShowYear;
    BOOL IsShowWeek;
    BOOL IsShowMonth;
    BOOL IsShowDay;
    BOOL IsShowHour;
    BOOL IsShowMinute;
    BOOL IsShowSecond;

    //int HourType;
    //int YearType;
    BOOL IsMutleLineShow;

}*LPDIGITALCLOCKAREAINFO;
//**页面显示的属性结构体****************************************************
struct PLAYPROP
{
    int			InStyle;	//入场特技值（取值范围 0-38） 具体查看文档
    int			OutStyle;	//退场特技值（现无效，预留，置0）
    int			Speed;		//特技显示速度(取值范围1-255)
    int			DelayTime;	//页面留停时间(1-65535)   注：当入场特技为连续左移、连续右移、连续上移、连续下移时，此参数无效
};
//*******************************************************************************

//**计时属性结构体**********************************************************************
typedef struct TIMEAREAINFO
{
    int         ShowFormat;             //显示格式  0.xx天xx时xx分xx秒  1.xx天xx時xx分xx秒  2.xxDayxxHourxxMinxxSec  3.XXdXXhXXmXXs  4.xx:xx:xx:xx
    int         nYear;                  //结束年
    int         nMonth;                 //结束月
    int         nDay;                   //结束日
    int         nHour;                  //结束时
    int         nMinute;                //结束分
    int         nSecond;                //结束秒
    BOOL        IsShowDay;              //是否显示天
    BOOL        IsShowHour;             //是否显示时
    BOOL        IsShowMinute;           //是否显示分
    BOOL        IsShowSecond;           //是否显示秒
    BOOL        IsMutleLineShow;        //是否多行显示，指的是自定义文字与计时文字是否分行显示
    TCHAR       ShowStr[128];           //自定义文字字符串
    COLORREF    TimeStrColor;           //计时文字的颜色
    FONTPROP    ShowFont;               //自定义文字及计时文字颜色，其中FontColor只对文定义文字有效，计时文字颜色为TimeStrColor
}* LPTIMEAREAINFO;


typedef int (*SERVERINFOCALLBACK)(int Msg, int wParam, void* lParam);
//回调消息枚举
enum
{
	LV_MSG_NONE,
	LV_MSG_CARD_ONLINE,
	LV_MSG_CARD_OFFLINE,
};
//回调参数结构体
typedef struct _card_info
{
	int port;
	char ipStr[16];
	char networkIdStr[19];
}CARD_INFO,*LP_CARD_INFO;


/********************************************************************************************
*   LV_LedInit            初始化缓存文件路径 如"/home/listenvision/"       注意：不调用此接口，缓存文件默认为执行程序的路径
*
*   参数说明
*               workPath        缓存文件路径
********************************************************************************************/
void LV_LedInit(const char *workPath);



/********************************************************************************************
*	LV_InitLed			初始化屏的类型和颜色顺序(C卡)
*  当Led上显示的文字区域的颜色与下发的不一致, 请的确认Led 屏的RGB顺序,并调用此接口
*	参数说明
*				nLedType		屏类型  0.6代T系A系XC系    1.6代E系     2.X1X2		3.7代C系 4: E5,E6
*				nRgb		模组的RGB顺序,仅C卡有效,其他卡固定为0. C卡时, 0:  R->G->B 1: G->R->B 2:R->B->G 3:B->R->G 4:B->G->R 5:G->B->R
*	返回值   无
*				
********************************************************************************************/
void LV_InitLed(int nLedType, int nRgb);

/********************************************************************************************
*	LV_CreateProgramEx			创建节目对象，返回类型为 HPROGRAM
*
*	参数说明
*				LedWidth		屏的宽度
*				LedHeight		屏的高度
*				ColorType		屏的颜色 1.单色  2.双基色  3.三基色   注：C卡全彩参数为3      X系列卡参数固定为 4
*				GrayLevel		灰度等级  赋值  1-5对应的灰度等级分别为 无,4,8,16,32   注意：6代T系A系XC系必须为0
*				SaveType		节目保存位置，默认为0保存为flash节目，3保存为ram节目。注：flash节目掉电不清除，ram节目掉电清除。应用场景需要实时刷新的，建议保持为ram节目
*	返回值
*				0				创建节目对象失败
*				非0				创建节目对象成功
********************************************************************************************/
HPROGRAM LV_CreateProgramEx(int LedWidth,int LedHeight,int ColorType,int GrayLevel,int SaveType);
/*********************************************************************************************
*	LV_AddProgram				添加一个节目
*
*	参数说明
*				hProgram		节目对象句柄
*				ProgramNo		节目号，从0开始(0-255)
*				ProgramTime		节目播放时长 0.节目播放时长  非0.指定播放时长
*				LoopCount		循环播放次数
*	返回值
*				0				成功
*				非0				失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_AddProgram(HPROGRAM hProgram,int ProgramNo,int ProgramTime,int LoopCount);

/*********************************************************************************************
*	LV_AddImageTextArea				添加一个图文区域
*
*	参数说明
*				hProgram			节目对象句柄
*				ProgramNo			节目号  从0开始(0-255)
*				AreaNo				区域号	(1-255)
*				pAreaRect			区域坐标属性，设置方式见AREARECT结构体注示
*				IsBackgroundArea	是否为背景区域，0.前景区（默认） 1.背景区  注：除C系列，其它默认为1
*	返回值
*				0					成功
*				非0					失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_AddImageTextArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,BOOL IsBackgroundArea);
/*********************************************************************************************
*	LV_AddFileToImageTextArea			添加一个文件到图文区
*
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号  从0开始(0-255)
*				AreaNo					区域号	(1-255)
*				FilePath				文件路径，支持的文件类型有 txt  rtf  bmp jpg jpeg 
*				pPlayProp				显示的属性，设置方式见PLAYPROP结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_AddFileToImageTextArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPCTSTR FilePath,PLAYPROP *pPlayProp);
/*********************************************************************************************
*	LV_AddSingleLineTextToImageTextArea	添加一个单行文本到图文区
*
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号  从0开始(0-255)
*				AreaNo					区域号	(1-255)
*				AddType					添加的类型  0.为字符串  1.文件（只支持txt）
*				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径
*				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
*				pPlayProp				显示的属性，设置方式见PLAYPROP结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_AddSingleLineTextToImageTextArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,PLAYPROP *pPlayProp);
/*********************************************************************************************
*	LV_AddMultiLineTextToImageTextArea	添加一个多行文本到图文区
*
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号  从0开始(0-255)
*				AreaNo					区域号  (1-255)
*				AddType					添加的类型  0.为字符串  1.文件（只支持txt）
*				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径    换行符（\n）
*				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
*				pPlayProp				显示的属性，设置方式见PLAYPROP结构体注示
*				nAlignment				水平对齐样式，0.左对齐  1.水平居中  2.右对齐  （注意：只对字符串和txt文件有效）
*				IsVCenter				是否垂直居中  0.置顶（默认） 1.垂直居中
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int	LV_AddMultiLineTextToImageTextArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,PLAYPROP *pPlayProp,int nAlignment,BOOL IsVCenter);
/*********************************************************************************************
 *	LV_AddStaticTextToImageTextArea		添加一个静止文本到图文区
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号  从0开始(0-255)
 *				AreaNo					区域号	(1-255)
 *				AddType					添加的类型  0.为字符串  1.文件（只支持txt）
 *				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径
 *				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
 *				DelayTime				显示的时长 1~65535
 *				nAlignment				水平对齐样式，0.左对齐  1.水平居中  2.右对齐  （注意：只对字符串和txt文件有效）
 *				IsVCenter				是否垂直居中  0.置顶（默认） 1.垂直居中
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
int	LV_AddStaticTextToImageTextArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,int DelayTime,int nAlignment,BOOL IsVCenter);
/*********************************************************************************************
*	LV_QuickAddSingleLineTextArea		快速添加一个左移单行文本区域
*
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号  从0开始(0-255)
*				AreaNo					区域号	(1-255)
*				pAreaRect				区域坐标属性，设置方式见AREARECT结构体注示
*				AddType					添加的类型  0.为字符串  1.文件（只支持txt）
*				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径
*				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
*				nSpeed					滚动速度 1~255
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_QuickAddSingleLineTextArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,int nSpeed);

/*********************************************************************************************
*	LV_AddDigitalClockArea				添加一个数字时钟区域
*
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号 从0开始(0-255)
*				AreaNo					区域号 (1-255)
*				pAreaRect				区域坐标属性，设置方式见AREARECT结构体注示
*				pDigitalClockAreaInfo	数字时钟属性，见DIGITALCLOCKAREAINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_AddDigitalClockArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPDIGITALCLOCKAREAINFO pDigitalClockAreaInfo);

/*********************************************************************************************
 *  LV_AddTimeArea                      添加一个计时区域
 *  
 *  参数说明
 *              hProgram                节目对象句柄
 *              ProgramNo               节目号  从0开始(0-255)
 *              AreaNo                  区域号 (1-255)
 *              pAreaRect               区域坐标属性，设置方式见AREARECT结构体注示
 *              pTimeAreaInfo           计时属性，见TIMEAREAINFO结构体注示
 *  返回值
 *              0                       成功
 *              非0                      失败，调用LV_GetError来获取错误信息 
 ********************************************************************************************/
int LV_AddTimeArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPTIMEAREAINFO pTimeAreaInfo);

/*********************************************************************************************
 *  LV_AddNeiMaArea                     添加一个内码区域,局部文本刷新，卡型号是否支持和开发流程，请查看内码区域局部更新协议
 *  
 *  参数说明
 *              hProgram                节目对象句柄
 *              ProgramNo               节目号  从0开始(0-255)
 *              AreaNo                  区域号 (1-255)
 *              pAreaRect               区域坐标属性，设置方式见AREARECT结构体注示
 *              NeiMaStr                文本字符串   注：字符串编码是GB2312
 *              FontSize                字体大小 16 24 32
 *              FontColor               文字颜色 格式BBGGRR 0xff 红色  0xff00 绿色  0xffff黄色
 *              pPlayProp               显示的属性，设置方式见PLAYPROP结构体注示
 *  返回值
 *              0                       成功
 *              非0                      失败，调用LV_GetError来获取错误信息 
 ********************************************************************************************/
int LV_AddNeiMaArea(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPCTSTR NeiMaStr,int FontSize,int FontColor,PLAYPROP* pPlayProp);

/*********************************************************************************************
*	LV_DeleteProgram					销毁节目对象(注意：如果此节目对象不再使用，请调用此函数销毁，否则会造成内存泄露)
*
*	参数说明
*				hProgram				节目对象句柄
********************************************************************************************/
void LV_DeleteProgram(HPROGRAM hProgram);
/*********************************************************************************************
*	LV_Send								发送节目，此发送为一对一发送
*
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*				hProgram				节目对象句柄
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_Send(LPCOMMUNICATIONINFO pCommunicationInfo,HPROGRAM hProgram);


/*********************************************************************************************
*	LV_RefreshNeiMaArea								刷新内码区域
*	
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*				NeiMaStr				刷新的数据字符串,格式可以查看<<内码区域局部更新协议>>文档
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
int  LV_RefreshNeiMaArea(LPCOMMUNICATIONINFO pCommunicationInfo,LPCTSTR NeiNaStr);


/*********************************************************************************************
*	LV_SetBasicInfoEx						设置基本屏参
*
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*				ColorType				屏的颜色 1.单色  2.双基色  3.三基色   注：C卡全彩参数为3      X系列卡参数固定为 4
*				GrayLevel				灰度等级  赋值  1-5对应的灰度等级分别为 无,4,8,16,32  注：目前C系列的卡才支持，其它型号（T,A,U,XC,W,E,X）参数必须为0 
*				LedWidth				屏的宽度点数
*				LedHeight				屏的高度点数
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_SetBasicInfoEx(LPCOMMUNICATIONINFO pCommunicationInfo,int ColorType,int GrayLevel,int LedWidth,int LedHeight);
/*********************************************************************************************
*	LV_GetError								获取错误信息（只支持中文）
*
*	参数说明
*				nErrCode					函数执行返回的错误代码
*				nMaxCount					pErrStr字符串缓冲区的大小（为字符的个数，非字节数）
*				pErrStr						待获取错误信息的字符串地址
********************************************************************************************/
void LV_GetError(int nErrCode,int nMaxCount,OUT LPTSTR pErrStr);
/*********************************************************************************************
*	LV_PowerOnOff						设置屏开关
*
*	参数说明
*				pCommunicationInfo	通讯参数，赋值方式见LPCOMMUNICATIONINFO结构体注示
*				OnOff                   屏开关  0.开屏 1.关屏 2.重启 
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_PowerOnOff(LPCOMMUNICATIONINFO pCommunicationInfo, int OnOff);

/*********************************************************************************************
*	LV_AdjustTime						校时
*
*	参数说明
*				pCommunicationInfo	通讯参数，赋值方式见LPCOMMUNICATIONINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_AdjustTime(LPCOMMUNICATIONINFO pCommunicationInfo);

/*********************************************************************************************
*	LV_TestOnline						测试是否通讯正常
*
*	参数说明
*				pCommunicationInfo	通讯参数，赋值方式见LPCOMMUNICATIONINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息
********************************************************************************************/
int LV_TestOnline(LPCOMMUNICATIONINFO pCommunicationInfo);

/*********************************************************************************************
*	LV_LedInitServer			启动监听服务端口
*	
*	参数说明
*				port			监听的端口
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
int LV_InitServer(int port);

/*********************************************************************************************
*	LV_LedShudownServer			断开控制卡心跳包服务
********************************************************************************************/
int LV_LedShudownServer();

/*********************************************************************************************
*	LV_RegisterLedServerCallback			注册回调函数
*	
*	参数说明
*				ledServerCallback					回调函数

********************************************************************************************/
void LV_RegisterServerCallback(SERVERINFOCALLBACK ledServerCallback);

#endif

