from flask import Blueprint, jsonify, request
from app.models.ledInfo import LedInfo
from app import db

ledInfo_router = Blueprint('ledInfo_router', __name__)

@ledInfo_router.route('/leds', methods=['GET'])
def get_leds():
    leds = LedInfo.query.all()
    return jsonify([led.to_dict() for led in leds])


@ledInfo_router.route('/leds/<int:ledid>', methods=['GET'])
def get_ledinfo(ledid):
    ledInfo= LedInfo.query.get(ledid)
    if ledInfo:
        return jsonify(ledInfo.to_dict())
    else:
        return jsonify({'error': 'ledInfo not found'})

@ledInfo_router.route('/leds', methods=['POST'])
def create_ledinfo():
    data = request.get_json()
    ledInfo= LedInfo(ledid=data['ledid'], park_id=data['park_id'])
    db.session.add(ledInfo)
    db.session.commit()
    return jsonify(ledInfo.to_dict())

@ledInfo_router.route('/leds/<int:ledid>', methods=['PUT'])
def update_ledinfo(ledid):
    ledInfo= LedInfo.query.get(ledid)
    if ledInfo:
        data = request.get_json()
        ledInfo.ledid = data['ledid']
        ledInfo.park_id = data['park_id']
        db.session.commit()
        return jsonify(ledInfo.to_dict())
    else:
        return jsonify({'error': 'ledInfo not found'})

@ledInfo_router.route('/leds/<int:ledid>', methods=['DELETE'])
def delete_ledinfo(ledid):
    ledInfo= LedInfo.query.get(ledid)
    if ledInfo:
        db.session.delete(ledInfo)
        db.session.commit()
        return jsonify({'message': 'ledInfo deleted successfully'})
    else:
        return jsonify({'error': 'ledInfo not found'})

