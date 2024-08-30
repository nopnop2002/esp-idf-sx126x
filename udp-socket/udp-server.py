import sys
import select, socket
import time
#!/usr/bin/env python3
#-*- encoding: utf-8 -*-
import argparse
import socket
import signal

def handler(signal, frame):
	global running
	print('handler')
	running = False

if __name__ == "__main__":
	signal.signal(signal.SIGINT, handler)
	running = True

	parser = argparse.ArgumentParser()
	parser.add_argument('--port', type=int, help='udp port', default=8080)
	args = parser.parse_args()
	print("args.port={}".format(args.port))

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.bind(('', args.port))

	while running:
		result = select.select([sock],[],[],1)
		#print(result[0], type(result[0]), len(result[0]))
		if (len(result[0]) == 1):
			msg = result[0][0].recv(1024)
			if (type(msg) is bytes):
				msg=msg.decode('utf-8')
			print(msg)

