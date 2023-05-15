from http.server import BaseHTTPRequestHandler, HTTPServer
import time
import socketserver
import json
import requests


hostName = "0.0.0.0"
serverPort  = 18080
led_server="http://nps.hyman.store:11007/neima?key="

last_update_response =""
current_empty_plot =0
    

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>CheYun->ledServer</title></head>", "utf-8"))
        self.wfile.write(bytes("<p>Request: %s</p>" % self.path, "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        self.wfile.write(bytes(f"<p>last_update_response: {last_update_response}</p>", "utf-8"))
        self.wfile.write(bytes(f"<p>Current empty plot: {current_empty_plot}</p>", "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))
    
    def do_POST(self):
        print(self.path)
        if self.path!= "/out_park" and self.path!="/in_park":
            return
        
        content_length= self.headers.get('content-length')

        length = int(content_length) if content_length else 0
        body = self.rfile.read(length)
        
    
        try:
            json_body = json.loads(body.decode('utf-8'))
            current_empty_plot= json_body['data']['empty_plot']
    
            url = led_server+str(current_empty_plot)

            response = requests.get(url)
            print(response.text)
            last_update_response = response.text
            

        except Exception as e:
            print("Error parsing JSON body: ", e)


if __name__ == "__main__":        
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")