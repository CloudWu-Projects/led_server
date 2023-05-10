import requests
import time
# URL to connect to
url = 'http://127.0.0.1:38180/neima'


v=1234
while True:
    # Parameters to send with the request
    params = {'key': str(v)}

    # Send the GET request
    response = requests.get(url, params=params)

    # Print the current value of the 'key' parameter
    print('Original key value:', response.text)
    v=v+1
    time.sleep(1)

