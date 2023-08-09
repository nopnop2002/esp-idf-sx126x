#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import datetime
import socket
import requests

server = "esp32-server.local"
port = 8080
try:
    ip = socket.gethostbyname(server)
except socket.gaierror as e:
    print(f'Invalid hostname, error raised is {e}')
    exit()
ip = socket.gethostbyname(server)
print("ip={}".format(ip))
uri = "http://{}:{}/post".format(ip, port)
print("uri={}".format(uri))

timezone = datetime.timedelta(hours=9)

while True:
	dt_now = datetime.datetime.now(datetime.timezone(timezone))
	#payload = "{}".format(datetime.datetime.now())
	#payload = "{}".format(datetime.datetime.now(datetime.timezone(timezone)))
	payload = dt_now.strftime('%Y/%m/%d %H:%M:%S')
	#print(payload)
	response = requests.post(uri, data=payload)
	#print(response.status_code)
	print("{}-->{}".format(payload, response.text))
	#print(response.text)
	time.sleep(1)
