#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import requests
import datetime
import time

server = "http://esp32-server.local:8080/post"
timezone = datetime.timedelta(hours=9)

while True:
	dt_now = datetime.datetime.now(datetime.timezone(timezone))
	#payload = "{}".format(datetime.datetime.now())
	#payload = "{}".format(datetime.datetime.now(datetime.timezone(timezone)))
	payload = dt_now.strftime('%Y/%m/%d %H:%M:%S')
	print(payload)
	response = requests.post(server, data=payload)
	print(response.status_code)
	print(response.text)
	time.sleep(1)
