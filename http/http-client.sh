#!/bin/bash
#sudo apt install avahi-utils
#set -x
SERVER="esp32-server.local"
HOST=`avahi-resolve -4 -n ${SERVER} | cut -f 2`
echo $HOST
URI="http://${HOST}:8080/post"

while :
do
	payload=`date "+%Y/%m/%d %H:%M:%S"`
	#echo ${payload}
	echo curl -d "${payload}" ${URI}
	curl -d "${payload}" ${URI}
	echo ""
	sleep 1
done
