from flask import Flask, request
import time
import json
import requests
import  sqlite3

hostName = "0.0.0.0"
serverPort  = 18080
led_server="http://nps.hyman.store:11007/neima?key="
led_server_empty_plot="http://nps.hyman.store:11007/empty_plot"

last_update_response =""
current_empty_plot =0

dbfilePath="file:cheyun.db"
def create_database():
    # Connect to database (creates a new database if it doesn't exist)
    conn = sqlite3.connect(dbfilePath,uri=True)

    # Create the "leds" table with columns "ledid" and "parkid"
    conn.execute('''CREATE TABLE leds (
    ledid TEXT PRIMARY KEY NOT NULL,
    parkname TEXT NOT NULL    
);''')

    # Create the "parkinfo" table with columns "parkid", "id", and "pgmfilepath"
    conn.execute('''CREATE TABLE parkinfo
                     (parkname TEXT PRIMARY KEY  NOT NULL,
                      pgmfilepath TEXT NOT NULL);''')

    # Commit the changes and close the connection

    conn.execute('''INSERT INTO leds
                    (ledid, parkname)
                    VALUES('test_1234567', 'test孵化园');''')
    
    conn.execute('''INSERT INTO leds
                    (ledid, parkname)
                    VALUES('test_abcv12345', 'test孵化园');''')
    
    conn.execute('''
                    INSERT INTO parkinfo
                    (parkname, pgmfilepath)
                    VALUES('test孵化园', 'pgm.prj');''')
    conn.commit()
    conn.close()

def try_openDB():
    try:
        conn = sqlite3.connect(f'{dbfilePath}?mode=ro',uri=True)
        c= conn.cursor()
        c.execute('''select parkname,pgmfilepath from parkinfo;''')
        c.execute('''select ledid,parkname from leds;''')
        conn.close()
    except Exception as e:
        print(e)
        create_database()
    

app = Flask(__name__)


@app.route('/listdb', methods=['GET'])    
def handle_listdb(): 
    parkName=request.args.get("parkname")
    try:
        conn = sqlite3.connect(f'{dbfilePath}?mode=ro',uri=True)
        c = conn.cursor()
        sql = f'select ledid,a.parkname ,b.pgmfilepath from leds a,parkinfo b where a.parkname=b.parkname '
    
        if parkName is not None and parkName!="":
            sql += f'and a.parkname="{parkName}"'
        c.execute(sql)
        response=f'{parkName}<br>'
        for row in c.fetchall():
            response +=str(row)+'<br>'
        conn.close()
        return response
    except Exception as e:
        print(e)
        return str(e)

@app.route('/test', methods=['GET'])
@app.route('/', methods=['GET'])
def handle_root():
        global last_update_response
        global current_empty_plot

        if request.path=="/test":
            current_empty_plot=int(time.time())
        
        a=f"<p>last_update_response: {last_update_response}</p>"
        a+=f"<p>Current empty plot: {current_empty_plot}</p>"
        return a
        
def handle_park(self,parkName,empty_plot):
    global last_update_response
    global current_empty_plot
    
    current_empty_plot = empty_plot
    try:
        conn = sqlite3.connect(f'{dbfilePath}?mode=ro',uri=True)
        c = conn.cursor()
        sql = f'select ledid,a.parkname ,b.pgmfilepath from leds a,parkinfo b where a.parkname=b.parkname '
        parkName=""
        if parkName!="":
            sql += f'and a.parkname="{parkName}"'
        c.execute(sql)
        pgmfilepath=""
        ledids=""
        
        for row in c.fetchall():
            print(row)           
            ledids += str(row[0])+","
            pgmfilepath += str(row[2])
        conn.close()
        url = f'{led_server_empty_plot}?ledids={ledids}&empty_plot={empty_plot}&pgmfilepath={pgmfilepath}'
        response = requests.get(url)
        print(response.text)
        last_update_response = response.text            
        

    except Exception as e:
        print(e)
        return str(e)


@app.route('/out_park', methods=['POST'])  
@app.route('/in_park', methods=['POST']) 
def out_in_park():    
    body = request.json
    try:
        json_body = json.loads(body.decode('utf-8'))
        current_empty_plot= json_body['data']['empty_plot']    
        parkName=''
        handle_park(parkName,current_empty_plot)   
    except Exception as e:
        print("Error parsing JSON body: ", e)


if __name__ == "__main__":        
    try_openDB()
    app.run(host=hostName, port=serverPort)    
    print("Server started http://%s:%s" % (hostName, serverPort))
    
    print("Server stopped.")