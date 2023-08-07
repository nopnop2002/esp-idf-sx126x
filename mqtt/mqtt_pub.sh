#!/bin/bash
#set -x
count=1
while :
do
        payload=`date "+%Y/%m/%d %H:%M:%S"`
        #payload="count=${count}"
        echo ${payload}
        mosquitto_pub -h broker.emqx.io -p 1883 -t "/topic/lora/test" -m "$payload"
        count=$((++count))
        sleep 1
done

