
import  sqlite3
from flask import g

from config import *

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(dbfilePath,uri=True)
    return db


def query_db(query, args=(), one=False):
    cur = get_db().execute(query, args)
    rv = cur.fetchall()
    cur.close()
    return (rv[0] if rv else None) if one else rv


def create_database():
    # Connect to database (creates a new database if it doesn't exist)
    conn = sqlite3.connect(dbfilePath,uri=True)

    # Create the "leds" table with columns "ledid" and "parkid"
    conn.execute('''CREATE TABLE leds (
    ledid TEXT PRIMARY KEY NOT NULL,
    park_id integer NOT NULL    
);''')

    # Create the "parkinfo" table with columns "parkid", "id", and "pgmfilepath"
    conn.execute('''CREATE TABLE parkinfo
                     (park_id integer PRIMARY KEY  NOT NULL,
                    park_name TEXT NOT NULL,
                      pgmfilepath TEXT NOT NULL);''')

    # Commit the changes and close the connection

    conn.execute('''INSERT INTO leds
                    (ledid, park_id)
                    VALUES('960302311001506', 25082);''')
    
    conn.execute('''INSERT INTO leds
                    (ledid, park_id)
                    VALUES('860302250008951', 25082);''')
    
    conn.execute('''
                    INSERT INTO parkinfo
                    (park_id, park_name, pgmfilepath)
                    VALUES(25082,'test孵化园', 'single_area.lsprj');''')
    conn.commit()
    conn.close()


def try_openDB():
    try:
        conn = sqlite3.connect(f'{dbfilePath}?mode=ro',uri=True)
        c= conn.cursor()
        c.execute('''select park_id,pgmfilepath from parkinfo;''')
        c.execute('''select ledid,park_id from leds;''')
        conn.close()
    except Exception as e:
        print(e)
        create_database()
    
    
   
