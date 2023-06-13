from app import db

class ParkInfo(db.Model):
    park_id = db.Column(db.Integer, primary_key=True)
    park_name = db.Column(db.String(50))
    pgmfilepath = db.Column(db.String(50))

    def to_dict(self):
        return {
            'park_id': self.park_id,
            'park_name': self.park_name,
            'pgmfilepath': self.pgmfilepath
        }
