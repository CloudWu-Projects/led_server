
# Config

    从sqliteDB or Ini 读取 led对应列表  ledMap = map<车场号,list<led编号>>

    [车场号]
    led=led编号1,led编号2
    pgmfile= 车场号.lspj
    
    or  
    select pgmfile from db where 车场号='车场号'
    select led编号 from db where 车场号='车场号'


    
# LED

~~使用协议开发~~

使用 **SDK** 开发

监听10008 端口

获取客户端链接， 并记录到LedList=std::set<led编号> 中

下发命令




# HttpServer

启动一个http 端口 支持 httpRestAPI

    API ：

        empty_plot
           empty_plot?ledids={ledids}&empty_plot={empty_plot}&pgmfilepath={pgmfilepath}&park_id={park_id}&fontcolor={fontcolor}
				   
        
        updateLedContent
            updateLedContent?ledids={ledids}&parkid=车场号&1号库状态(areaName)=value1&2号库状态=value2

# 车场云 API

    https://k7plwkq2y5.k.topthink.com/@z7poqbk2e4/gaishu.html
    
# usage

车场云 
    --->chechangyun.py 
        ---->call http://xxxx/updateParkNumber
            ----> call LedServer updateParkNumber
                ----> 加载该车场对应的 pgm.lsprj
                    --->创建节目
                        ---->通过sdk 发送给对应的LED
