#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import socket
import requests
import argparse
import signal

def handler(signal, frame):
	global running
	print('handler')
	running = False

if __name__=='__main__':
	signal.signal(signal.SIGINT, handler)
	running = True

	parser = argparse.ArgumentParser()
	parser.add_argument('--host', help='tcp host', default="esp32-server.local")
	parser.add_argument('--port', type=int, help='tcp port', default=8080)
	args = parser.parse_args()
	print("args.host={}".format(args.host))
	print("args.port={}".format(args.port))

	try:
		ip = socket.gethostbyname(args.host)
		print("ip={}".format(ip))
	except socket.gaierror as e:
		print(f'Invalid hostname, error raised is {e}')
		exit()
	uri = "http://{}:{}/post".format(ip, args.port)
	print("uri={}".format(uri))

	while running:
		t = time.time()
		local_time = time.localtime(t)
		payload = time.asctime(local_time)
		print("payload=[{}]".format(payload))
		#print(type(payload))
		response = requests.post(uri, data=payload)
		#print(response.status_code)
		print("{}-->{}".format(payload, response.text))
		#print(response.text)
		time.sleep(1)
