#!/bin/bash
#Install websocat
#sudo wget -qO /usr/local/bin/websocat https://github.com/vi/websocat/releases/latest/download/websocat.x86_64-unknown-linux-musl
#sudo chmod a+x /usr/local/bin/websocat
#websocat --version

SERVER="esp32-server.local"
HOST=`avahi-resolve -4 -n ${SERVER} | cut -f 2`
echo $HOST
URI="ws://${HOST}:8080"

while :
do
	payload=`date "+%Y/%m/%d %H:%M:%S"`
	echo -n ${payload}"--->"
	#echo ${payload} | websocat -b ws://192.168.10.173:8080
	#echo -n "aaaaa" | websocat -b ${URI}
	echo -n ${payload} | websocat -b ${URI}
	echo ""
	sleep 1
done
