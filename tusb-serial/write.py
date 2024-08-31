#!/usr/bin/python3
#-*- encoding: utf-8 -*-
import sys
import argparse
import time
import serial
import signal

def handler(signal, frame):
	global running
	print('handler')
	running = False

if __name__=='__main__':
	signal.signal(signal.SIGINT, handler)
	running = True

	parser = argparse.ArgumentParser()
	parser.add_argument('--device', help='usb device', default="/dev/ttyACM0")
	parser.add_argument('--interval', type=int, help='send interval', default=1)
	args = parser.parse_args()
	print("args.device={}".format(args.device))
	print("args.interval={}".format(args.interval))

	try:
		ser = serial.Serial(args.device, 115200, timeout=None)
	except:
		print("Unable to open {}".format(args.device))
		sys.exit()

	while running:
		local_time = time.localtime()
		asc_time = time.asctime(local_time) + "\r\n"
		bytes_time = bytes(asc_time, 'utf-8')
		ser.write(bytes_time)
		time.sleep(args.interval)

	ser.close()
