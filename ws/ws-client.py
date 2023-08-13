#!/usr/bin/env python
# python3 -m pip install websockets
# https://github.com/python-websockets/websockets

import time
import datetime
import socket
from websockets.sync.client import connect

server = "esp32-server.local"
port = 8080
try:
	ip = socket.gethostbyname(server)
except socket.gaierror as e:
	print(f'Invalid hostname, error raised is {e}')
	exit()
print("ip={}".format(ip))
uri = "ws://{}:{}".format(ip, port)
print("uri={}".format(uri))

timezone = datetime.timedelta(hours=9)
websocket = connect("ws://esp32-server.local:8080")

while True:
	dt_now = datetime.datetime.now(datetime.timezone(timezone))
	payload = dt_now.strftime('%Y/%m/%d %H:%M:%S')
	websocket.send(payload)
	responce = websocket.recv()
	print("{}-->{}".format(payload, responce))
	time.sleep(1.0)

