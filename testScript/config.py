
import os
import platform

hostName = "0.0.0.0"
serverPort  = 18080
led_server="http://nps.hyman.store:11007/neima?key="
led_server_empty_plot="http://nps.hyman.store:11007/empty_plot"





sysstr = platform.system()
baseConfigPath='/home/admin/cheyun/'


webPagePath=f'{baseConfigPath}webPage/'
if(sysstr =="Windows"):
    a = os.path.split(os.path.realpath(__file__))
    baseConfigPath=a[0]
    webPagePath=f'{baseConfigPath}/../webPage/'
    
elif(sysstr == "Linux"):
    print ("Call Linux tasks")
print(baseConfigPath)
upload_folder = f'{baseConfigPath}/upload/'

dbfilePath=f"file:{baseConfigPath}/cheyun.db"