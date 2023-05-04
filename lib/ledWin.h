#pragma once

#include <windows.h>
#include <stdint.h>

typedef void* LPVOID;

typedef LPVOID				HPROGRAM;	//节目句柄类型
typedef char TCHAR;
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

typedef int (*SERVERINFOCALLBACK)(int Msg, long wParam, void* lParam);
enum
{
	LV_MSG_NONE,
	LV_MSG_CARD_ONLINE,
	LV_MSG_CARD_OFFLINE,
};

typedef struct _card_info
{
	int port;
	TCHAR ipStr[16];
	TCHAR networkIdStr[19];
}CARD_INFO,*LP_CARD_INFO;

//**通讯设置结构体*********************************************************
typedef struct COMMUNICATIONINFO
{
	int		LEDType;				//LED类型	0.6代T系A系XC系    1.6代E系     2.X1X2		3.C系
	int		SendType;				//通讯方式	0.为Tcp发送（又称固定IP通讯）,		1.广播发送（又称单机直连）		2.串口通讯		3.磁盘保存    4.广域网通讯
	TCHAR	IpStr[16];				//LED屏的IP地址，只有通讯方式为0时才需赋值，其它通讯方式无需赋值
	int		Commport;				//串口号，只有通讯方式为2时才需赋值，其它通讯方式无需赋值
	int		Baud;					//波特率，只有通讯方式为2时才需赋值，其它通讯方式无需赋值,   0.9600   1.57600   2.115200  直接赋值 9600，19200，38400，57600，115200亦可
	int		LedNumber;				//LED的屏号，只有通讯方式为2时，且用485通讯时才需赋值，其它通讯方式无需赋值
	TCHAR	OutputDir[MAX_PATH];	//磁盘保存的目录，只有通讯方式为3时才需赋值，其它通讯方式无需赋值
	TCHAR	NetworkIdStr[19];		//唯一网路 ID  注：只有C2M \C4M 广义网开发才需赋值。其它无需赋值
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
	TCHAR		FontName[32];		//字体名
	int			FontSize;			//字号(单位磅)
	COLORREF	FontColor;			//字体颜色
	BOOL		FontBold;			//是否加粗
	BOOL		FontItalic;			//是否斜体
	BOOL		FontUnderLine;		//时否下划线
};
//****************************************************************************


//**页面显示的属性结构体****************************************************
struct PLAYPROP 
{
	int			InStyle;	//入场特技值（取值范围 0-38）
	int			OutStyle;	//退场特技值（现无效，预留，置0）
	int			Speed;		//特技显示速度(取值范围1-255)
	int			DelayTime;	//页面留停时间(1-65535)    注：当入场特技为连续左移、连续右移、连续上移、连续下移时，此参数无效
};
      /*  特技值对应
            0=立即显示
            1=随机
            2=左移
            3=右移
            4=上移
            5=下移
            6=连续左移
            7=连续右移
            8=连续上移
            9=连续下移
            10=闪烁
            11=激光字(向上)
            12=激光字(向下)
            13=激光字(向左)
            14=激光字(向右)
            15=水平交叉拉幕
            16=上下交叉拉幕
            17=左右切入
            18=上下切入
            19=左覆盖
            20=右覆盖
            21=上覆盖
            22=下覆盖
            23=水平百叶(左右)
            24=水平百叶(右左)
            25=垂直百叶(上下)
            26=垂直百叶(下上)
            27=左右对开
            28=上下对开
            29=左右闭合
            30=上下闭合
            31=向左拉伸
            32=向右拉伸
            33=向上拉伸
            34=向下拉伸
            35=分散向左拉伸
            36=分散向右拉伸
            37=冒泡
            38=下雪
         */
//*******************************************************************************

//**设置节目定时属性结构体****************************************************
typedef struct PROGRAMTIME
{
	int				EnableFlag;		//启用定时的标记，ENABLE_DATE为启用日期,ENABLE_TIME为启用时间,ENABLE_WEEK为启用星期,可用或运算进行组合，如 ENABLE_DATE | ENABLE_TIME | ENABLE_WEEK
	int				WeekValue;		//启用星期后，选择要定时的星期里的某些天，用宏 WEEK_MON,WEEK_TUES,WEEK_WEN,WEEK_THUR,WEEK_FRI,WEEK_SAT,WEEK_SUN 通过或运算进行组合
	int				StartYear;		//起始年
	int				StartMonth;		//起始月
	int				StartDay;		//起始日
	int				StartHour;		//起姐时
	int				StartMinute;	//起始分
	int				StartSecond;	//起始秒
	int				EndYear;		//结束年
	int				EndMonth;		//结束月
	int				EndDay;			//结束日
	int				EndHour;		//结束时
	int				EndMinute;		//结束分
	int				EndSecond;		//结束秒
}*LPPROGRAMTIME;
//**********************************************************************************

//数字时钟属性结构体*********************************************************************************
typedef struct DIGITALCLOCKAREAINFO
{
	TCHAR		ShowStr[128];			//自定义显示字符串
	FONTPROP	ShowStrFont;			//自定义显示字符串以及日期星期时间的字体属性，注意此字体属性里的FontColor只对自定义显示字体有效，其它项的颜色有单独的颜色属性，属性的赋值见FONTPROP结构体说明
	int			TimeLagType;			//时差类型 0为超前，1为滞后
	int			HourNum;				//时差小时数	
	int			MiniteNum;				//时差分钟数
	int			DateFormat;				//日期格式 0.YYYY年MM月DD日  1.YY年MM月DD日  2.MM/DD/YYYY  3.YYYY/MM/DD  4.YYYY-MM-DD  5.YYYY.MM.DD  6.MM.DD.YYYY  7.DD.MM.YYYY
	COLORREF	DateColor;				//日期字体颜色		格式是16进制 BBGGRR（如：红色0xff 绿色0xff00 黄色0xffff）
	int			WeekFormat;				//星期格式 0.星期X  1.Monday  2.Mon.
	COLORREF	WeekColor;				//星期字体颜色
	int			TimeFormat;				//时间格式 0.HH时mm分ss秒  1.HH時mm分ss秒  2.HH:mm:ss  3.上午 HH:mm:ss  4.AM HH:mm:ss  5.HH:mm:ss 上午  6.HH:mm:ss AM
	COLORREF	TimeColor;				//时间字体颜色
	BOOL		IsShowYear;				//是否显示年 TRUE为显示 FALSE不显示 下同
	BOOL		IsShowWeek;				//是否显示星期
	BOOL		IsShowMonth;			//是否显示月
	BOOL		IsShowDay;				//是否显示日
	BOOL		IsShowHour;				//是否显示时
	BOOL		IsShowMinute;			//是否显示分
	BOOL		IsShowSecond;			//是否显示秒
	BOOL		IsMutleLineShow;		//是否多行显示
}*LPDIGITALCLOCKAREAINFO;
//******************************************************************************

//**模拟时钟属性结构体*********************************************************
typedef struct CLOCKAREAINFO
{
	TCHAR		ShowStr[64];			//自定义显示字符串
	FONTPROP	ShowStrFont;			//自定义显示字符串字体属性
	int			TimeLagType;			//时差类型 0为超前，1为滞后
	int			HourNum;				//时差小时数
	int			MiniteNum;				//时差分钟数
	int			ClockType;				//表盘类型  0.圆形  1.正方形
	COLORREF	HourMarkColor;			//时标颜色		格式是16进制 BBGGRR（如：红色0xff 绿色0xff00 黄色0xffff）
	int			HourMarkType;			//时标类型	0.圆形  1.正方形
	int			HourMarkWidth;			//时标宽度	1~16
	COLORREF	MiniteMarkColor;		//分标颜色
	int			MiniteMarkType;			//分标类型	0.圆形  1.正方形
	int			MiniteMarkWidth;		//分标宽度  1~16
	COLORREF	HourPointerColor;		//时针颜色
	COLORREF	MinutePointerColor;		//分针颜色
	COLORREF	SecondPointerColor;		//秒针颜色
	int			HourPointerWidth;		//时针的宽度  1~5
	int			MinutePointerWidth;		//分针的宽度  1~5
	int			SecondPointerWidth;		//秒针的宽度  1~5
	BOOL		IsShowDate;				//是否显示日期	
	int			DateFormat;				//日期格式 0.YYYY年MM月DD日  1.YY年MM月DD日  2.MM/DD/YYYY  3.YYYY/MM/DD  4.YYYY-MM-DD  5.YYYY.MM.DD  6.MM.DD.YYYY  7.DD.MM.YYYY
	FONTPROP	DateFont;				//日期字体属性
	BOOL		IsShowWeek;				//是否显示星期
	int			WeekFormat;				//星期格式 0.星期X  1.Monday  2.Mon.
	FONTPROP	WeekFont;				//星期字体属性
}*LPCLOCKAREAINFO;
//**************************************************************************************

//**计时属性结构体**********************************************************************
typedef struct TIMEAREAINFO
{
	int			ShowFormat;				//显示格式	0.xx天xx时xx分xx秒  1.xx天xx時xx分xx秒  2.xxDayxxHourxxMinxxSec  3.XXdXXhXXmXXs  4.xx:xx:xx:xx
	int			nYear;					//结束年
	int			nMonth;					//结束月
	int			nDay;					//结束日
	int			nHour;					//结束时
	int			nMinute;				//结束分
	int			nSecond;				//结束秒
	BOOL		IsShowDay;				//是否显示天
	BOOL		IsShowHour;				//是否显示时
	BOOL		IsShowMinute;			//是否显示分
	BOOL		IsShowSecond;			//是否显示秒
	BOOL		IsMutleLineShow;		//是否多行显示，指的是自定义文字与计时文字是否分行显示
	TCHAR		ShowStr[128];			//自定义文字字符串
	COLORREF	TimeStrColor;			//计时文字的颜色
	FONTPROP	ShowFont;				//自定义文字及计时文字颜色，其中FontColor只对文定义文字有效，计时文字颜色为TimeStrColor
}* LPTIMEAREAINFO;
//****************************************************************************************



//**LED通讯参数修改结构体*****************************************************************
typedef struct LEDCOMMUNICATIONPARAMETER
{
	DWORD	dwMask;				//要修改项的标记  0.修改网络通讯参数  1.修改串口通讯参数  2.修改网口和串口通讯参数
	TCHAR	IpStr[16];			//新的IP地址，只有dwMask为0或2时才需赋值，其它值无需赋值，格式例如 192.168.1.100
	TCHAR	NetMaskStr[16];		//新的子网掩码，只有dwMask为0或2时才需赋值，其它值无需赋值，格式例如 255.255.255.0
	TCHAR	GatewayStr[16];		//新的网关，只有dwMask为0或2时才需赋值，其它值无需赋值,格式例如 192.168.1.1
	TCHAR	MacStr[18];			//新的MAC地址，只有dwMask为0或2时才需赋值，其它值无需赋值，格式例如 12-34-56-78-9a-bc,如无需修改请设为 ff-ff-ff-ff-ff-ff
	int		Baud;				//波特率，只有dwMask为1或2时才需赋值，其它值无需赋值，0.9600  1.57600  2.115200
	int		LedNumber;			//LED屏号 1~255,网络通讯和232通讯赋值 1 即可，485必需和控制卡显示的屏号相同才可通讯
}*LPLEDCOMMUNICATIONPARAMETER;
//*****************************************************************************************


//**流水边框属性结构体************************************************************************
typedef struct WATERBORDERINFO
{
	int			Flag;							//流水边框加载类型标志，0.为动态库预置的边框  1.为从文件加载的边框
	int			BorderType;						//边框的类型，Flag为0是有效，0.单色边框  1.双基色边框  2.全彩边框
	int			BorderValue;					//边框的值，Flag为0是有效，单色边框取值范围是0~39,双基色边框取值范围是0~34,全彩边框取值范围是0~21
	COLORREF	BorderColor;					//边框线颜色,Flag为0并且BorderType为0是才有效
	int			BorderStyle;					//边框显示的样式  0.固定  1.顺时针  2.逆时针  3.闪烁
	int			BorderSpeed;					//边框流动的速度
	TCHAR		WaterBorderBmpPath[MAX_PATH];	//边框图片文件的路径，注意只能是bmp图片，图片大小必需是宽度为32点，取高度小于等于8
}*LPWATERBORDERINFO;
//*********************************************************************************************

typedef struct VOICEAREAINFO
{
//	CString		VoiceStr;						//文字
	int			DelayTime;						//间隔时间
	int			PlayCount;						//播放速度
}*LPVOICEAREAINFO;

/********************************************************************************************
 *	LV_CreateProgramEx			创建节目对象，返回类型为 HPROGRAM
 *
 *	参数说明
 *				LedWidth		屏的宽度
 *				LedHeight		屏的高度
 *				ColorType		屏的颜色 1.单色  2.双基色  3.三基色   注：C卡全彩参数为3      X系列卡参数固定为 4
 *				GrayLevel		灰度等级  赋值  1-5对应的灰度等级分别为 无,4,8,16,32  注：目前C系列的卡才支持，其它型号（T,A,U,XC,W,E,X）参数必须为0
 *				SaveType		节目保存位置，默认为0保存为flash节目，3保存为ram节目 
								注：flash节目掉电不清除，ram节目掉电清除。应用场景需要实时刷新的，建议保存为ram节目
 *	返回值
 *				0				创建节目对象失败
 *				非0				创建节目对象成功
 ********************************************************************************************/
typedef	HPROGRAM	(__stdcall *_LV_CreateProgramEx)(int LedWidth,int LedHeight,int ColorType,int GrayLevel,int SaveType);

/*********************************************************************************************
 *	LV_AddProgram				添加一个节目
 *	
 *	参数说明
 *				hProgram		节目对象句柄
 *				ProgramNo		节目号 （取值范围0-255)（从0开始）
 *				ProgramTime		节目播放时长 0.节目播放时长  非0.指定播放时长
 *				LoopCount		循环播放次数   1-255
 *	返回值
 *				0				成功
 *				非0				失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddProgram)(HPROGRAM hProgram,int ProgramNo,int ProgramTime,int LoopCount);

/*********************************************************************************************
 *	LV_SetProgramTime			设置节目定时
 *	
 *	参数说明
 *				hProgram		节目对象句柄
 *				ProgramNo		节目号 （取值范围0-255)（从0开始）
 *				pProgramTime	节目定时属性，设置方式见PROGRAMTIME结构体注示
 *	返回值
 *				0				成功
 *				非0				失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_SetProgramTime)(HPROGRAM hProgram,int ProgramNo,LPPROGRAMTIME pProgramTime);

/*********************************************************************************************
 *	LV_AddImageTextArea				添加一个图文区域
 *	
 *	参数说明
 *				hProgram			节目对象句柄
 *				ProgramNo			节目号 （取值范围0-255)（从0开始）
 *				AreaNo				区域号 （取值范围1-255）
 *				pAreaRect			区域坐标属性，设置方式见AREARECT结构体注示
 *				IsBackgroundArea	是否为背景区域，0.前景区（默认） 1.背景区  注：除C系列，其它默认为1
 *	返回值
 *				0					成功
 *				非0					失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddImageTextArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,BOOL IsBackgroundArea);


/*********************************************************************************************
 *	LV_AddFileToImageTextArea			添加一个文件到图文区
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				FilePath				文件路径，支持的文件类型有 txt  rtf  bmp  gif  png  jpg jpeg tiff
 *				pPlayProp				显示的属性，设置方式见PLAYPROP结构体注示
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddFileToImageTextArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPCTSTR FilePath,PLAYPROP *pPlayProp);

/*********************************************************************************************
 *	LV_AddSingleLineTextToImageTextArea	添加一个单行文本到图文区
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				AddType					添加的类型  0.为字符串  1.文件（只支持txt和rtf文件）
 *				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径
 *				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
 *				pPlayProp				显示的属性，设置方式见PLAYPROP结构体注示
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddSingleLineTextToImageTextArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,PLAYPROP *pPlayProp);

/*********************************************************************************************
 *	LV_AddMultiLineTextToImageTextArea	添加一个多行文本到图文区
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				AddType					添加的类型  0.为字符串  1.文件（只支持txt和rtf文件）
 *				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径    换行符（\n）
 *				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
 *				pPlayProp				显示的属性，设置方式见PLAYPROP结构体注示
 *				nAlignment				水平对齐样式，0.左对齐  1.右对齐  2.水平居中  （注意：只对字符串和txt文件有效）
 *				IsVCenter				是否垂直居中  0.置顶（默认） 1.垂直居中
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddMultiLineTextToImageTextArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,PLAYPROP *pPlayProp,int nAlignment,BOOL IsVCenter);

/*********************************************************************************************
 *	LV_AddStaticTextToImageTextArea		添加一个静止文本到图文区
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				AddType					添加的类型  0.为字符串  1.文件（只支持txt和rtf文件）
 *				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径
 *				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
 *				DelayTime				显示的时长 1~65535
 *				nAlignment				水平对齐样式，0.左对齐  1.右对齐  2.水平居中  （注意：只对字符串和txt文件有效）
 *				IsVCenter				是否垂直居中  0.置顶（默认） 1.垂直居中
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddStaticTextToImageTextArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,int DelayTime,int nAlignment,BOOL IsVCenter);

/*********************************************************************************************
 *	LV_QuickAddSingleLineTextArea		快速添加一个向左移的单行文本区域
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				pAreaRect				区域坐标属性，设置方式见AREARECT结构体注示
 *				AddType					添加的类型  0.为字符串  1.文件（只支持txt和rtf文件）
 *				AddStr					AddType为0则为字符串数据,AddType为1则为文件路径
 *				pFontProp				如果AddType为字符串类型或AddType为文件类型且文件为txt则可传入以赋值的该结构体，其它可赋NULL
 *				nSpeed					滚动速度 1~255 值越大，速度越慢
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_QuickAddSingleLineTextArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,int AddType,LPCTSTR AddStr,FONTPROP *pFontProp,int nSpeed);

/*********************************************************************************************
 *	LV_AddDigitalClockArea				添加一个数字时钟区域
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				pAreaRect				区域坐标属性，设置方式见AREARECT结构体注示
 *				pDigitalClockAreaInfo	数字时钟属性，见DIGITALCLOCKAREAINFO结构体注示
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddDigitalClockArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPDIGITALCLOCKAREAINFO pDigitalClockAreaInfo);

/*********************************************************************************************
 *	LV_AddTimeArea						添加一个计时区域
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 *				ProgramNo				节目号 （取值范围0-255)（从0开始）
 *				AreaNo					区域号 （取值范围1-255）
 *				pAreaRect				区域坐标属性，设置方式见AREARECT结构体注示
 *				pTimeAreaInfo			计时属性，见TIMEAREAINFO结构体注示
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_AddTimeArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPTIMEAREAINFO pTimeAreaInfo);

/*********************************************************************************************
*	LV_AddVoiceArea						添加一个语音区域
*	
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号 （取值范围0-255)（从0开始）
*				AreaNo					区域号 （取值范围1-255）
*				pVoiceAreaInfo			语音属性，见VOICEAREAINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_AddVoiceArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPVOICEAREAINFO pVoiceAreaInfo);

/*********************************************************************************************
*	LV_AddNeiMaArea						添加一个内码区域
*	
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号 （取值范围0-255)（从0开始）
*				AreaNo					区域号 （取值范围1-255）
*				pAreaRect				区域坐标属性，设置方式见AREARECT结构体注示
*				NeiMaStr                文本字符串 
*				FontSize                字体大小 16 24 32
*				FontColor               文字颜色 格式BBGGRR 0xff 红色  0xff00 绿色  0xffff黄色
*				pPlayProp               显示的属性，设置方式见PLAYPROP结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_AddNeiMaArea)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPCTSTR NeiNaStr,int FontSize,int FontColor,PLAYPROP* pPlayProp);

/*********************************************************************************************
*	LV_AddWaterBorder						添加一个边框
*	
*	参数说明
*				hProgram				节目对象句柄
*				ProgramNo				节目号 （取值范围0-255)（从0开始）
*				AreaNo					区域号 （取值范围1-255）
*				pWaterBorderInfo		边框属性，见LPWATERBORDERINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_AddWaterBorder)(HPROGRAM hProgram,int ProgramNo,int AreaNo,LPAREARECT pAreaRect,LPWATERBORDERINFO pWaterBorderInfo);

/*********************************************************************************************
 *	LV_DeleteProgram					销毁节目对象(注意：如果此节目对象不再使用，请调用此函数销毁，否则会造成内存泄露)
 *	
 *	参数说明
 *				hProgram				节目对象句柄
 ********************************************************************************************/
typedef	void		(__stdcall *_LV_DeleteProgram)(HPROGRAM hProgram);


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
typedef	int			(__stdcall *_LV_Send)(LPCOMMUNICATIONINFO pCommunicationInfo,HPROGRAM hProgram);

/*********************************************************************************************
 *	LV_MultiSendOne						发送节目，此发送为多块屏共享一个节目对象并行发送
 *	
 *	参数说明
 *				pCommunicationInfoArray	通讯参数，为一数组，赋值方式见COMMUNICATIONINFO结构体注示
 *				hProgram				节目对象句柄
 *				pResultArray			发送返回的结果数组,函数返回后通过此值判断发送是否成功，0为成功，非0失败（调用LV_GetError来获取错误信息）	
 *				LedCount				要发送的屏的个数，即为pCommunicationInfoArray和pResultArray数组的上标数
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_MultiSendOne)(LPCOMMUNICATIONINFO pCommunicationInfoArray,HPROGRAM hProgram,OUT int* pResultArray,int LedCount);

/*********************************************************************************************
 *	LV_MultiSend						发送节目，此发送为多块屏发送不同的节目，并行发送
 *	
 *	参数说明
 *				pCommunicationInfoArray	通讯参数，为一数组，赋值方式见COMMUNICATIONINFO结构体注示
 *				phProgramArray			节目对象句柄数组
 *				pResultArray			发送返回的结果数组,函数返回后通过此值判断发送是否成功，0为成功，非0失败（调用LV_GetError来获取错误信息）	
 *				LedCount				要发送的屏的个数，即为pCommunicationInfoArray、phProgramArray和pResultArray数组的上标数
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_MultiSend)(LPCOMMUNICATIONINFO pCommunicationInfoArray,HPROGRAM *phProgramArray,OUT int* pResultArray,int LedCount);

/*********************************************************************************************
 *	LV_SetBasicInfoEx						设置基本屏参
 *	
 *	参数说明
 *				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
 *				ColorType				屏的颜色 1.单色  2.双基色  3.三基色    注：C卡全彩参数为3      X系列卡参数固定为 4
 *				GrayLevel				灰度等级  赋值  1-5对应的灰度等级分别为 无,4,8,16,32  注：目前C系列的卡才支持，其它型号（T,A,U,XC,W,E,X）参数必须为0 
 *				LedWidth				屏的宽度点数
 *				LedHeight				屏的高度点数
 *	返回值
 *				0						成功
 *				非0						失败，调用LV_GetError来获取错误信息	
 ********************************************************************************************/
typedef	int			(__stdcall *_LV_SetBasicInfoEx)(LPCOMMUNICATIONINFO pCommunicationInfo,int ColorType,int GrayLevel,int LedWidth,int LedHeight);

/*********************************************************************************************
*	LV_TestOnline						测试LED屏是否可连接上
*	
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_TestOnline)(LPCOMMUNICATIONINFO pCommunicationInfo);

/*********************************************************************************************
*	LV_SetOEDA							设置OE DA
*	
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*				Oe						OE  0.低有效  1.高有效
*				Da						DA  0.负极性  1.正极性
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_SetOEDA)(LPCOMMUNICATIONINFO pCommunicationInfo,int Oe,int Da);

/*********************************************************************************************
*	LV_AdjustTime						校时
*	
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_AdjustTime)(LPCOMMUNICATIONINFO pCommunicationInfo);

/*********************************************************************************************
*	LV_PowerOnOff						开关屏
*	
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*				OnOff					开关值  0.关屏  1.开屏  2.重启
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_PowerOnOff)(LPCOMMUNICATIONINFO pCommunicationInfo,int OnOff);

/*********************************************************************************************
*	LV_SetBrightness					设置亮度
*	
*	参数说明
*				pCommunicationInfo		通讯参数，赋值方式见COMMUNICATIONINFO结构体注示
*				BrightnessValue			亮度值 0~15
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_SetBrightness)(LPCOMMUNICATIONINFO pCommunicationInfo,int BrightnessValue);

/*********************************************************************************************
 *	LV_GetError								获取错误信息（只支持中文）
 *	
 *	参数说明
 *				nErrCode					函数执行返回的错误代码
 *				nMaxCount					pErrStr字符串缓冲区的大小（为字符的个数，非字节数）
 *				pErrStr						待获取错误信息的字符串地址
 ********************************************************************************************/
typedef void		(__stdcall *_LV_GetError)(int nErrCode,int nMaxCount,OUT LPTSTR pErrStr);


/*********************************************************************************************
*	LV_LedInitServer			启动控制卡心跳包服务 注：只有C2M、C4M才支持
*	
*	参数说明
*				port			监听的端口
*	返回值
*				0						成功
*				非0						失败，调用LV_GetError来获取错误信息	
********************************************************************************************/
typedef	int			(__stdcall *_LV_LedInitServer)(int port);

/*********************************************************************************************
*	LV_ShudownServer			断开控制卡心跳包服务  注：只有C2M、C4M才支持
********************************************************************************************/
typedef void		(__stdcall *_LV_LedShudownServer)(void);

/*********************************************************************************************
*	LV_RegisterLedServerCallback			注册回调函数  注：只有C2M、C4M才支持
*	
*	参数说明
*				serverCallback					回调函数指针

********************************************************************************************/
typedef void		(__stdcall *_LV_RegisterLedServerCallback)(SERVERINFOCALLBACK serverCallback);

