from app import db

class LedInfo(db.Model):
    ledid = db.Column(db.Integer, primary_key=True)
    park_id = db.Column(db.Integer)

    def to_dict(self):
        return {
            'ledid': self.ledid,
            'park_id': self.park_id
        }
