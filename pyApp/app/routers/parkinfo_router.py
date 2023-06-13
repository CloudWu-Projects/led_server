from flask import Blueprint, jsonify, request
from app.models.parkInfo import ParkInfo
from app import db

parkinfo_router = Blueprint('parkinfo_router', __name__)

@parkinfo_router.route('/parks', methods=['GET'])
def get_parks():
    parks = ParkInfo.query.all()
    return jsonify([park.to_dict() for park in parks])


@parkinfo_router.route('/parks/<int:park_id>', methods=['GET'])
def get_park(park_id):
    parkInfo= ParkInfo.query.get(park_id)
    if parkInfo:
        return jsonify(parkInfo.to_dict())
    else:
        return jsonify({'error': 'parkInfo not found'})

@parkinfo_router.route('/leds', methods=['POST'])
def create_park():
    data = request.get_json()
    parkInfo= ParkInfo(park_id=data['park_id'], park_name=data['park_name'], pgmfilepath=data['pgmfilepath'])
    db.session.add(parkInfo)
    db.session.commit()
    return jsonify(parkInfo.to_dict())

@parkinfo_router.route('/parks/<int:park_id>', methods=['PUT'])
def update_park(park_id):
    parkInfo= ParkInfo.query.get(park_id)
    if parkInfo:
        data = request.get_json()
        parkInfo.park_id = data['park_id']
        parkInfo.park_name = data['park_name']
        parkInfo.pgmfilepath = data['pgmfilepath']
        db.session.commit()
        return jsonify(parkInfo.to_dict())
    else:
        return jsonify({'error': 'parkInfo not found'})

@parkinfo_router.route('/parks/<int:park_id>', methods=['DELETE'])
def delete_park(park_id):
    parkInfo= ParkInfo.query.get(park_id)
    if parkInfo:
        db.session.delete(parkInfo)
        db.session.commit()
        return jsonify({'message': 'parkInfo deleted successfully'})
    else:
        return jsonify({'error': 'parkInfo not found'})

