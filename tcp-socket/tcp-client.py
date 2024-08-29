#!/usr/bin/python3
#-*- encoding: utf-8 -*-
import sys
import argparse
import socket
import time
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
	parser.add_argument('--interval', type=int, help='send interval', default=1)
	parser.add_argument('--retry', type=int, help='connection retry', default=3)
	args = parser.parse_args()
	print("args.host={}".format(args.host))
	print("args.port={}".format(args.port))
	print("args.interval={}".format(args.interval))
	print("args.retry={}".format(args.retry))

	enable_ip_address= False
	if ('.local' in args.host):
		try:
			ip_address = socket.gethostbyname(args.host)
			print("ip_address={}".format(ip_address))
			enable_ip_address = True
		except:
			print("Unable to connect to {}".format(args.host))
			sys.exit()

	retry = 0
	while running:
		tcp_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		tcp_client.settimeout(10)

		try:
			if (enable_ip_address == True):
				tcp_client.connect((ip_address, args.port))
			else:
				tcp_client.connect((args.host, args.port))
			print("connected to {}".format(args.host))
			retry = 0
		except:
			print("Unable to connect to {}".format(args.host))
			retry = retry + 1
			if (retry == args.retry):
				sys.exit()
			continue

		t = time.time()
		local_time = time.localtime(t)
		asc_time = time.asctime(local_time)
		#print("asc_time={}".format(type(asc_time)))
		bytes_time = bytes(asc_time, 'utf-8')
		tcp_client.send(bytes_time)
		tcp_client.close()
		time.sleep(args.interval)

