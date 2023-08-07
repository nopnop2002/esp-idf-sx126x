#!/bin/bash
mosquitto_sub -d -h broker.emqx.io  -t "/topic/lora/test"
