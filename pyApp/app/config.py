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

dbfilePath=f"sqlite:///{baseConfigPath}/cheyun.db"

class Config:
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'secret-key'
    SQLALCHEMY_DATABASE_URI = os.environ.get('DATABASE_URL') or \
        dbfilePath
    SQLALCHEMY_TRACK_MODIFICATIONS = False
