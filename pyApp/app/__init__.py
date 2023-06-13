from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from .config import Config
from flask import jsonify

app = Flask(__name__)

app.config.from_object(Config)
db = SQLAlchemy(app)

# Import models
from .models.user import User
from .models.ledInfo import LedInfo
from .models.parkInfo import ParkInfo

# Import routers
from .routers.user_router import user_router
from .routers.ledInfo_router import ledInfo_router
from .routers.parkinfo_router import parkinfo_router

# Register routers
app.register_blueprint(user_router)
app.register_blueprint(ledInfo_router)
app.register_blueprint(parkinfo_router)



@app.route('/list', methods=['GET'])
def index():
    routes=[]
    for rule in app.url_map.iter_rules():
        routes.append(f'{rule.methods} {rule.rule}')
    return jsonify(routes)

@app.route("/createdb", methods=["GET"])
def createdb():
    db.create_all()
    return "Database created"

if __name__ == '__main__':
    app.run(debug=True)