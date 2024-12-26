#!/usr/bin/env python
# python3 -m pip install websockets
# https://github.com/python-websockets/websockets

import time
import socket
from websockets.sync.client import connect
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
	except socket.gaierror as e:
		print(f'Invalid hostname, error raised is {e}')
		exit()
	print("ip={}".format(ip))
	uri = "ws://{}:{}".format(ip, args.port)
	print("uri={}".format(uri))
	websocket = connect(uri)

	while running:
		t = time.time()
		local_time = time.localtime(t)
		payload = time.asctime(local_time)
		websocket.send(payload)
		responce = websocket.recv()
		print("{}-->{}".format(payload, responce))
		time.sleep(1.0)

