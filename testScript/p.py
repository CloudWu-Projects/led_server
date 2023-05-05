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
