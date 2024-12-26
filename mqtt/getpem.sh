#!/bin/bash
#
# Extract the root certificate from ${HOST}:${PORT}.

#set -x

#HOST="mqtt.eclipseprojects.io"
HOST="broker.emqx.io"
PORT="8883"

openssl s_client -showcerts -connect $HOST:$PORT </dev/null >hoge

start=`grep -e "-----BEGIN CERTIFICATE-----" -n hoge | sed -e 's/:.*//g' | tail -n 1`

last=`grep -e "-----END CERTIFICATE-----" -n hoge | sed -e 's/:.*//g' | tail -n 1`

sed -n ${start},${last}p hoge > main/root_cert.pem

rm hoge
