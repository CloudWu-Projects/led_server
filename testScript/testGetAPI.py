import requests

# Get API
ledids='1234'
park_id='1234'
dat={
    "ledids":[
        "960302311001506"
    ],
    "pgmfilepath":"D:\\ledpgm\\lanfengled.lsprj",
    "park_id":park_id,
    "fontSize": 5,
    "fontColor": 0xff00ee,
     
    "store_1_status": 1 ,
    "store_2_status": 2 ,
    "store_3_status": 3 ,
    "store_4_status": 1 ,
    "store_5_status": 2 ,
    "store_6_status": 3 ,
    "content":{        
        "store_1_status":"1",
        "store_2_status":"2",
        "store_3_status":"3",
        "store_4_status":"1",
        "store_5_status":"2",
        "store_6_status":"3",
    },       
    "areas":[
        {
            "name":"title",
            "value":"1",            
            "fontSize": 20,
            "fontColor": 0xff
        },
        {
            "name":"store_1_status",
            "value":"1",            
            "fontSize": 20,
            "fontColor": 0xff
        },
        {
            "name":"store_2_status",
            "value":"1",       
        },
        {
            "name":"store_3_status",
            "value":"1",      
        },
        {
            "name":"store_4_status",
            "value":"1",       
        },
        {
            "name":"store_5_status",
            "value":"1",        
        },
        {
            "name":"store_6_status",
            "value":"1",      
        },
        {
            "name":"store_7_status",
            "value":"1",       
        },
        {
            "name":"store_8_status",
            "value":"1",  
        },
        {
            "name":"store_2_status",
            "value":"1",     
        },
    ]
    
}
dat={
    "ledids":[
        "192.168.123.199"
    ],
    "isip":True,
    "pgmfilepath":"D:\\ledpgm\\lanfengled.lsprj",
    "park_id":park_id,
    "fontSize": 5,
    "fontColor": 0xff,
     
    "store_status_1": 3 ,
    "store_status_2": 1 ,
    "store_status_3": 1 ,
    "store_status_4": 3 ,
    "store_status_5": 3 ,
    "store_status_6": 3 ,
    "store_status_7": 2 ,
    "store_status_8": 3 ,
}
'{\n  "args": {\n    "content": [\n      "1\\u53f7\\u5e93\\u72b6\\u6001", \n      "2\\u53f7\\u5e93\\u72b6\\u6001", \n      "3\\u53f7\\u5e93\\u72b6\\u6001", \n      "4\\u53f7\\u5e93\\u72b6\\u6001", \n      "5\\u53f7\\u5e93\\u72b6\\u6001", \n      "6\\u53f7\\u5e93\\u72b6\\u6001"\n    ], \n    "ledids": "1234", \n    "park_id": "1234"\n  }, \n  "headers": {\n    "Accept": "*/*", \n    "Accept-Encoding": "gzip, deflate", \n    "Host": "httpbin.org", \n    "User-Agent": "python-requests/2.29.0", \n    "X-Amzn-Trace-Id": "Root=1-649f714d-210fda017b25be075ba867b1"\n  }, \n  "origin": "103.192.227.110", \n  "url": "http://httpbin.org/get?ledids=1234&park_id=1234&content=1\\u53f7\\u5e93\\u72b6\\u6001&content=2\\u53f7\\u5e93\\u72b6\\u6001&content=3\\u53f7\\u5e93\\u72b6\\u6001&content=4\\u53f7\\u5e93\\u72b6\\u6001&content=5\\u53f7\\u5e93\\u72b6\\u6001&content=6\\u53f7\\u5e93\\u72b6\\u6001"\n}\n'
#http://httpbin.org/get?ledids=1234&park_id=1234&content=1\\u53f7\\u5e93\\u72b6\\u6001&content=2\\u53f7\\u5e93\\u72b6\\u6001&content=3\\u53f7\\u5e93\\u72b6\\u6001&content=4\\u53f7\\u5e93\\u72b6\\u6001&content=5\\u53f7\\u5e93\\u72b6\\u6001&content=6\\u53f7\\u5e93\\u72b6\\u6001

b = requests.get('http://127.0.0.1:11007/updateLedContent_LanfengLED',json=dat)
print(b.text)

c = b.json()
print(b.json())