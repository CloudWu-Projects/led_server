from flask import Flask, request,Response,render_template,g
from flask_restful import Api,Resource
import time
import json
import requests
import  sqlite3
import os
import logging
from dbFunc import *;
last_update_response =""
last_parkJson=""
last_updateLED=""
last_updateLED_time=0
last_update_time=0
current_empty_plot =0
from config import *;

if not os.path.exists(upload_folder):
    os.makedirs(upload_folder)

if(sysstr=="Linux"):
    logging.basicConfig(filename=f'{baseConfigPath}py_record.log',
                    level=logging.DEBUG, format='%(asctime)s %(levelname)s %(name)s %(threadName)s : %(message)s')

app = Flask(__name__,template_folder=webPagePath)

app.config['UPLOAD_FOLDER'] = upload_folder


@app.route('/test', methods=['GET'])
@app.route('/', methods=['GET'])
def handle_root():
    global last_update_response
    global current_empty_plot
    global last_update_time
    global last_parkJson

    if request.path=="/test":
        current_empty_plot=int(time.time())
    
    a=f"<p>last_update_response: {last_update_response}</p>"
    a+=f"<p> Current empty plot: {current_empty_plot}</p>"
    a+=f"<p>   last_update_time: {last_update_time}</p>"
    a+=f"<p>      last_parkJson: {last_parkJson}</p>"
    a+=f"<p>      last_updateLED: {last_updateLED}</p>"
    a+=f"<p> last_updateLED_time: {last_updateLED_time}</p>"

    return a
        
def handle_park(park_id,empty_plot):
    global last_update_response
    global current_empty_plot
    global last_update_time
    
    current_empty_plot = empty_plot
    try:        
        c = get_db().cursor()
        sql = f'select ledid,a.park_id,b.park_name ,b.pgmfilepath from leds a,parkinfo b where a.park_id=b.park_id '
        if park_id!="":
            sql += f'and a.park_id={park_id}'
        print(sql)
        c.execute(sql)
        pgmfilepath=""
        ledids=""        
        for row in c.fetchall():                       
            ledids += str(row[0])+","
            pgmfilepath = str(row[3])
        c.close()

        color = 0xff00 # Green
        if empty_plot<=10:
            color= 0xff # Red
        dat={
            "ledids":ledids,
            "empty_plot":empty_plot,
            "pgmfilepath":pgmfilepath,
            "park_id":park_id,
            "fontcolor":color
        }
        app.logger.debug(f'url: {led_server_empty_plot}')
        app.logger.debug(dat)

        response = requests.get(led_server_empty_plot,params=dat)
        last_update_response = response.text        
        last_update_time = time.asctime(time.localtime() )          

        app.logger.debug(response.text.encode('utf-8'))
        ajson = json.loads(response.text)
        for a in ajson['idlist']:
            if a in ledids:
                continue
            print(f"{a} no in {ledids}")
            requests.post(f'http://127.0.0.1:{serverPort}/api/ledinfo/{a}',params={'ledid':a,'park_id':-1})
        
        

    except Exception as e:
        print(e)
        return str(e)

@app.route('/updateLED',methods=['POST'])
def updateLED():
    json_body = request.json
    global last_updateLED
    global last_updateLED_time
    last_updateLED_time = time.asctime(time.localtime() )  
    app.logger.debug(json.dumps(json_body).encode('utf-8'))
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
    try:
        last_updateLED = json.dumps(json_body)
        # 找出parkid 对应的led记录 【ledid，ledpgmpath】
        park_id=json_body['park_id']
        LED_id = json_body['LED_id']

        c = get_db().cursor()
        sql = f'select ledid,a.park_id,b.park_name ,b.pgmfilepath from leds a,parkinfo b where a.park_id=b.park_id '
        if park_id!="":
            sql += f'and a.park_id={park_id}'
        if LED_id!="":
            sql+= f' and a.ledid="{LED_id}"'
            
        print(sql)
        c.execute(sql)
        pgmfilepath=""
        ledids=""        
        for row in c.fetchall():                       
            ledids += str(row[0])+","
            pgmfilepath = str(row[3])
        c.close()

    except Exception as e:
        return str(e)



@app.route('/out_park', methods=['POST'])  
@app.route('/in_park', methods=['POST']) 
def out_in_park():    
    json_body = request.json
    global last_parkJson    
    try:
       # json_body = json.loads(body.decode('utf-8'))
        last_parkJson=json.dumps(json_body)
        current_empty_plot= json_body['data']['empty_plot']    
        park_id=json_body['park_id']
        app.logger.debug(json.dumps(json_body).encode('utf-8'))

        handle_park(park_id,current_empty_plot)   
        reuslt={
  "state": 1,
  "order_id": json_body['data']['order_id'],
  "park_id": park_id,
  "service_name": json_body['service_name'],
  "errmsg": " send success!"
}
        return json.dumps(reuslt)
    except Exception as e:
        print("Error parsing JSON body: ", e)
        return str(e)

@app.route('/a', methods=['GET'])
def handle_temple():        
    leds =query_db('''select ledid,park_id from leds;''') 
    parkinfos = query_db('''select park_id,park_name,pgmfilepath from parkinfo;''')
    return render_template('a.html', title='Welcome', leds=leds,parkinfos=parkinfos)

@app.route('/all', methods=['GET'])
def handle_led_infos():
    try:            
        leds =query_db('''select ledid,park_id from leds;''')        
        response=f'<section> <section><div><h1>LEDs</h1><ul>'
        for row in leds:
            #formHtml=f'''<input type="submit" value="delete" onclick="deleteLed({row[0]})">'''
            response +=f'<li>{{row[0]}}   '+str(row)+'</li>'
        response+='</ul></div></section>'
        response+='<section><div>" " </div></section>'
        parkinfos = query_db('''select park_id,park_name,pgmfilepath from parkinfo;''')
        response+='<section><div><h1>Parks</h1><ul>'
        for row in parkinfos:  
            formHtml=f'''<input type="submit" value="delete" onclick="deletepark({row[0]})">'''          
            response +='<li>'+formHtml+str(row)+'</li>'
        response+='</ul></div></section></section>'

        rHtml='''            
        <html>
        <script>
        function setLedAction(form){
            form.action = "/api/ledinfo/"+form.ledid.value;
            fetch(form.action, {method:'post', body: new FormData(form)})
            .then(() =>{
            window.location.reload();
            } );
            return false;
        }
         function setparkAction(form){
                form.action = "/api/parkinfo/"+form.park_id.value;
                fetch(form.action, {method:'post', body: new FormData(form)})
            .then(() =>{
            window.location.reload();
            } );
            return false;
            }       
        

        function deleteLed(ledid){
            let deleteurl = "/api/ledinfo/"+ledid;
            fetch(deleteurl, {method: "DELETE"}) .then(() =>{
            window.location.reload();
            } );
        }
        function deletepark(parkid){
            let deleteurl = "/api/parkinfo/"+parkid;
            fetch(deleteurl, {method: "DELETE"})
            .then(() =>{
            window.location.reload();
            } );
        }

        </script>
        <link rel="stylesheet" href="https://unpkg.com/mvp.css@1.12/mvp.css"> 
        <body>
        <section>
            <section>
                <div><h1>ADD LEDs </h1>
                <form  onsubmit = "return setLedAction(this)" method="POST">	
                <p>ledid: <input type = "text" name = "ledid" />
                <p>Park id: <input type = "text" name = "park_id" />
                <input type="text" name="actiontype" value="add" hidden>
                <p><input type = "submit" value = "Add" />
                </form>
                </div>
            </section>
            <section>
                <div><h1>update LEDs </h1>
                <form  onsubmit = "return setLedAction(this)" method="POST">	
                <p>ledid: <input type = "text" name = "ledid" />
                <p>Park id: <input type = "text" name = "park_id" />
                <input type="text" name="actiontype" value="update" hidden>
                <p><input type = "submit" value = "update" />
                </form>
                </div>
            </section>
        <section><div>" "</div></section>
        <section>
            <div><h1>ADD Parks </h1>
            <form enctype = "multipart/form-data" onsubmit = "return setparkAction(this)" method="POST">	
                <p>Park Name: <input type = "text" name = "park_name" /></p>
                <p>Park id: <input type = "text" name = "park_id" /></p>
                <p>pgm File: <input type = "file" name = "file" /></p>
                <input type="text" name="actiontype" value="add" hidden>	
                <p><input type = "submit" value = "Add" /></p>
            </form>
            </div>
        </section>
        <section>
            <div><h1>update pgmfile </h1>
            <form enctype = "multipart/form-data" onsubmit = "return setparkAction(this)" method="POST">	
                <p>Park id: <input type = "text" name = "park_id" /></p>
                <p>pgm File: <input type = "file" name = "file" /></p>	
                <input type="text" name="actiontype" value="update" hidden>
                <p><input type = "submit" value = "update" /></p>
            </form>
            </div>
        </section>
        </section>
        ''';

        rHtml+=f'''<p>{response}</p>            </body>            </html>'''
        resp = Response(rHtml,mimetype='text/html')
        return resp
    except Exception as e:
        print(e)
        return str(e)
    
class LedInfo(Resource):
    def get(self,led_id):
        try:            
            sql=f'''select ledid,park_id from leds where ledid={led_id};'''
            response=f''
            for row in query_db(sql):
                response +=str(row)+','
            return response
        except Exception as e:
            print(e)
            return str(e)
    
    def put(self,led_id):
        try:            
            conn = sqlite3.connect(f'{dbfilePath}',uri=True)
            c = conn.cursor()
            c.execute(f'''update leds set park_id={request.values['park_id']} where ledid={led_id};''')
            conn.commit()
            conn.close()
            return 'ok'
        except Exception as e:
            print(e)
            return str(e)
        
    def post(self,led_id):
        try:
            park_id = request.values['park_id']            

            conn = sqlite3.connect(f'{dbfilePath}',uri=True)
            c = conn.cursor()
            if request.values['actiontype']=='add': 
                c.execute(f'''insert into leds(park_id , ledid) values({park_id},{led_id});''')
            else:
                c.execute(f'''update leds set park_id={park_id} where ledid={led_id};''')
            conn.commit()
            conn.close()
            return 'ok'
        except Exception as e:
            print(e)
            return str(e)
    
    def delete(self,led_id):
        try:            
            conn = sqlite3.connect(f'{dbfilePath}',uri=True)
            c = conn.cursor()
            c.execute(f'''delete from leds where ledid={led_id};''')
            conn.commit()
            conn.close()
            return 'ok'
        except Exception as e:
            print(e)
            return str(e)

class ParkInfo(Resource):
    def get(self,park_id):
        try:            
            sql = f'''select park_id,pgmfilepath,park_name from parkinfo where park_id={park_id};'''
        
            response=f'{park_id}<br>'

            for row in query_db(sql):
                response +=str(row)+'--->'
            return response
        except Exception as e:
            print(e)
            return str(e)
        
    def post(self,park_id):
        try:
            file = request.files['file']
            if not file:
                return "no file"
            
            lsprjfile = os.path.join(app.config['UPLOAD_FOLDER'], f'{park_id}.lsprj')
            file.save(lsprjfile)

            if request.values['actiontype']=='add':  
                park_name= request.values['park_name']                      
                conn = sqlite3.connect(f'{dbfilePath}',uri=True)
                c = conn.cursor()
                c.execute('''insert into parkinfo(park_id,pgmfilepath,park_name) values(?,?,?)''',(park_id,lsprjfile,park_name))
                conn.commit()
                conn.close()

            elif request.values['actiontype']=='update':
                conn = sqlite3.connect(f'{dbfilePath}',uri=True)
                c = conn.cursor()
                c.execute('''update parkinfo set pgmfilepath=? where park_id=?''',(lsprjfile,park_id))
                conn.commit()
                conn.close()

            return 'ok'
        except Exception as e:
            print(e)
            return str(e)
    def delete(self,park_id):
        try:            
            conn = sqlite3.connect(f'{dbfilePath}',uri=True)
            c = conn.cursor()
            c.execute(f'''delete from parkinfo where park_id={park_id};''')
            conn.commit()
            conn.close()
            return 'ok'
        except Exception as e:
            print(e)
            return str(e)


api = Api(app)
api.add_resource(LedInfo, '/api/ledinfo/<int:led_id>')
api.add_resource(ParkInfo, '/api/parkinfo/<int:park_id>')

if __name__ == "__main__":         
    try_openDB()
    print("Server started http://%s:%s" % (hostName, serverPort))
    app.run(host=hostName, port=serverPort)    
    print("Server stopped.")