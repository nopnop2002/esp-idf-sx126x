#!/bin/bash
#set -x
HOST=`getent ahostsv4 esp32-server.local | awk '{print $1}' | head -1`
#HOST="esp32-server.local"
PORT="8080"
while :
do
	payload=`date "+%Y/%m/%d %H:%M:%S"`
	echo ${payload}
	#echo -n ${payload} | nc -u -q1 ${HOST} ${PORT}
	echo -n ${payload} | nc -u -w1 ${HOST} ${PORT}
	sleep 1
done
