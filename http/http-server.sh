#!/bin/bash
PORT=8080
echo "Listening on port" ${PORT}
while true
do
	( echo "HTTP/1.0 200 Ok"; echo; echo -n "OK") | nc -l -p ${PORT} -w 1
	echo ""
	echo ""
done
