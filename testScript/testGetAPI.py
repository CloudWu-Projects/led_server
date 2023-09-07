import requests

# Get API
ledids='1234'
park_id='1234'
dat={
    "ledids":[
        "960302311001506",
        "192.168.123.199"
    ],
    "isip":True,
    "pgmfilepath":"D:\\ledpgm\\lanfengled.lsprj",
    "park_id":park_id,
    "fontSize": 5,
    "fontColor": 0xff00ee,     
    "areas":[
        {
            "name":"标题",
            "value":"兰丰水泥散泥库装车状态",            
            "fontSize": 20,
            "fontColor": 0xff
        },
        {
            "name":"1号库状态",
            "value":"1",            
            "fontSize": 20,
            "fontColor": 0xff
        },
        {
            "name":"2号库状态",
            "value":"1",       
        },
        {
            "name":"3号库状态",
            "value":"1",      
        },
        {
            "name":"4号库状态",
            "value":"1",       
        },
        {
            "name":"5号库状态",
            "value":"1",        
        },
        {
            "name":"6号库状态",
            "value":"停 用",      
        },
        {
            "name":"7号库状态",
            "value":"1",       
        },
        {
            "name":"8号库状态",
            "value":"待装车",  
        },
        {
            "name":"9号库状态",
            "value":"装车中",     
        },
        {
            "name":"1号库",
            "value":"1号库",  
        },
        {
            "name":"2号库",
            "value":"2号库",     
        },
    ]    
}
'{\n  "args": {\n    "content": [\n      "1\\u53f7\\u5e93\\u72b6\\u6001", \n      "2\\u53f7\\u5e93\\u72b6\\u6001", \n      "3\\u53f7\\u5e93\\u72b6\\u6001", \n      "4\\u53f7\\u5e93\\u72b6\\u6001", \n      "5\\u53f7\\u5e93\\u72b6\\u6001", \n      "6\\u53f7\\u5e93\\u72b6\\u6001"\n    ], \n    "ledids": "1234", \n    "park_id": "1234"\n  }, \n  "headers": {\n    "Accept": "*/*", \n    "Accept-Encoding": "gzip, deflate", \n    "Host": "httpbin.org", \n    "User-Agent": "python-requests/2.29.0", \n    "X-Amzn-Trace-Id": "Root=1-649f714d-210fda017b25be075ba867b1"\n  }, \n  "origin": "103.192.227.110", \n  "url": "http://httpbin.org/get?ledids=1234&park_id=1234&content=1\\u53f7\\u5e93\\u72b6\\u6001&content=2\\u53f7\\u5e93\\u72b6\\u6001&content=3\\u53f7\\u5e93\\u72b6\\u6001&content=4\\u53f7\\u5e93\\u72b6\\u6001&content=5\\u53f7\\u5e93\\u72b6\\u6001&content=6\\u53f7\\u5e93\\u72b6\\u6001"\n}\n'
#http://httpbin.org/get?ledids=1234&park_id=1234&content=1\\u53f7\\u5e93\\u72b6\\u6001&content=2\\u53f7\\u5e93\\u72b6\\u6001&content=3\\u53f7\\u5e93\\u72b6\\u6001&content=4\\u53f7\\u5e93\\u72b6\\u6001&content=5\\u53f7\\u5e93\\u72b6\\u6001&content=6\\u53f7\\u5e93\\u72b6\\u6001

b = requests.get('http://127.0.0.1:11007/updateLedContent_LanfengLED',json=dat)
print(b.text)

c = b.json()
print(b.json())