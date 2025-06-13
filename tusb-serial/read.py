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
	args = parser.parse_args()
	print("args.device={}".format(args.device))

	try:
		ser = serial.Serial(args.device, 115200, timeout=1)
	except:
		print("Unable to open {}".format(args.device))
		sys.exit()

	ser.reset_input_buffer()
	while running:
		msg = ser.readline()
		#print(len(msg))
		if (len(msg) == 0): continue
		if (type(msg) is bytes):
			msg=msg.decode('utf-8')
		print(msg, end='') 

	ser.close()
