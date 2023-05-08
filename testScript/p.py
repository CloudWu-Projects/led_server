import requests
import json

url = 'http://nps.hyman.store:28080'

# Send GET request to /
response = requests.get(url)
print('GET /:', response.text)

# Send POST request to /set with JSON data
data = {'key': 'value'}
headers = {'Content-type': 'application/json'}
response = requests.post(url + '/set', data=json.dumps(data), headers=headers)
print('POST /set:', response.text)


newPgm={'LED':{
        'LedWidth':0,
		'LedHeight':0,
		'LedType':0,
		'LedColor':0,
		'LedGray':0,
    'pgm':{
        'Area':[
           {
            "AreaNo": 1,
            "AreaRect_Left": 0,
            "AreaRect_Top": 0,
            "AreaRect_Right": 40,
            "AreaRect_Bottom": 40,
            "InSpeed": 0,
            "InStyle": 0,
            "OutStyle": 0,
            "DelayTime": 0,
            "FontColor": 0,
            "BackColor": 0
            }
        ]
    }
}}