import requests
import json

url = 'http://nps.hyman.store:18080'


data ={'data': {'work_station_uuid': '', 'flag': 0, 'electronic_pay': '0.0', 'electronic_prepay': '', 'remark': '', 'pic_addr': 'http://ist-falcon.oss-cn-shenzhen.aliyuncs.com/order-images/25082/out/1084996502.jpg?Expires=1685667343&OSSAccessKeyId=LTAIQQrl6GICP0QX&Signature=MWNc0U91zlc%2BnEEpDAYHoHjHRAw%3D', 'empty_plot': 429, 'car_type': '', 'duration': 1149, 'total': '0.0', 'charge_status': 0, 'license_color': 0, 'out_uid': '9', 'freereasons': '', 'out_operator_name': '刘敏', 'lock_key': 0, 'pay_type': 'cash', 'etc_pay': '', 'reduce_amount': '0.0', 'out_time': 1685580886, 'reduction_rules': '', 'cash_pay': '0.0', 'in_channel_id': '2', 'islocked': 1, 'ticket_id': '', 'auth_code': '', 'cash_prepay': '', 'in_time': 1685511922, 'out_type': '月租卡', 'c_type': '月租车', 'car_number': '川Q669F8', 'amount_receivable': '0.0', 'out_channel_id': '3', 'order_id': '1084996502', 'source': 'ist'}, 'service_name': 'out_park', 'park_id': 25082, 'sign': '773CD0A0B8F4523E53AD27B4EA4689FD', 'sendCount': 4}


response = requests.post(url + '/in_park', data=json.dumps(data))