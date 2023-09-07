import json
json_template={
            "service_name":"Receive_LED",
            "park_id":"12345678",
            "sign":"",
            "order_id":"10001",
            "LED_id":"11112312135",
            "data": [
                {
                    "F_id":1,
                    "F_message":"car1 coming",
                    "F_color":1
                },
                {
                    "F_id":2,
                    "F_message":"car2 coming",
                    "F_color":1
                }
            ]
        }

import hashlib
 
# 该函数实现对指定字符串取hash
def md5sum(str):
    m = hashlib.md5()
    m.update(str.encode("utf-8"))
    return m.hexdigest()
 

del json_template["sign"]
a = json.dumps(json_template, separators=(",", ":"),sort_keys=True)
b = json.dumps(json_template, sort_keys=True)

asign = md5sum(a)
bsign = md5sum(b)

print(a)