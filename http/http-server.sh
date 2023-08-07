#!/bin/bash
PORT=8080
echo "Listening on port" ${PORT}
while true
do
	# Header & payload
	#( echo "HTTP/1.0 200 Ok"; echo; echo -n "OK") | nc -l -p ${PORT} -w 1

	# Payload only
	( echo "HTTP/1.0 200 Ok"; echo; echo -n "OK") | nc -l -p ${PORT} -w 1 | tail -n 1
	echo ""
done
