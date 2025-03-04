# MQTT Example   
This is LoRa and MQTT gateway application.   
```
           +----------+           +----------+           +----------+           +----------+
           |          |           |          |           |          |           |          |
           |Publisher |--(MQTT)-->|  Broker  |--(MQTT)-->|  ESP32   |--(SPI)--->|  SX126x  |==(LoRa)==>
           |          |           |          |           |          |           |          |
           +----------+           +----------+           +----------+           +----------+

           +----------+           +----------+           +----------+           +----------+
           |          |           |          |           |          |           |          |
==(LoRa)==>|  SX126x  |--(SPI)--->|  ESP32   |--(MQTT)-->|  Broker  |--(MQTT)-->|Subscriber|
           |          |           |          |           |          |           |          |
           +----------+           +----------+           +----------+           +----------+
```


# Configuration   
![config-app](https://user-images.githubusercontent.com/6020549/162125873-556d92c4-60aa-46fb-9f6e-d2ff38858f58.jpg)
![Image](https://github.com/user-attachments/assets/75ba296b-9bee-4aba-8643-0acb6317d7db)

## WiFi Setting
![config-mqtt-2](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/df4c9cb5-c11d-488f-a951-4c0148e8b847)

## Radio Setting

### MQTT to LoRa   
 Subscribe with MQTT and send to LoRa.   
 You can use mosquitto_pub as Publisher.   
 ```mosquitto_pub -h broker.emqx.io -p 1883 -t "/topic/lora/test" -m "test"```

```
           +----------+           +----------+           +----------+           +----------+
           |          |           |          |           |          |           |          |
           |Publisher |--(MQTT)-->|  Broker  |--(MQTT)-->|  ESP32   |--(SPI)--->|  SX126x  |==(LoRa)==>
           |          |           |          |           |          |           |          |
           +----------+           +----------+           +----------+           +----------+
```

![Image](https://github.com/user-attachments/assets/60b7535f-7e21-4c1e-8fe0-352a55295892)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_RX   

### LoRa to MQTT   
 Receive from LoRa and publish as MQTT.   
 You can use mosquitto_sub as Subscriber.   
 ```mosquitto_sub -h broker.emqx.io -p 1883 -t "/topic/lora/test"```

```
           +----------+           +----------+           +----------+           +----------+
           |          |           |          |           |          |           |          |
==(LoRa)==>|  SX126x  |--(SPI)--->|  ESP32   |--(MQTT)-->|  Broker  |--(MQTT)-->|Subscriber|
           |          |           |          |           |          |           |          |
           +----------+           +----------+           +----------+           +----------+
```

![Image](https://github.com/user-attachments/assets/23700f3a-58a9-471f-8528-c3b144a89447)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   


### Select Transport   
This project supports TCP,SSL/TLS,WebSocket and WebSocket Secure Port.   
- Using TCP Port.   
	TCP Port uses the MQTT protocol.   
	![Image](https://github.com/user-attachments/assets/88d81143-cd7c-4044-8d7f-1191b98c4051)

- Using SSL/TLS Port.   
	SSL/TLS Port uses the MQTTS protocol instead of the MQTT protocol.   
	![Image](https://github.com/user-attachments/assets/9fe68ebf-f4b5-484a-b82c-95e8d9d64572)

- Using WebSocket Port.   
	WebSocket Port uses the WS protocol instead of the MQTT protocol.   
	![Image](https://github.com/user-attachments/assets/75ca9dfe-30ac-49d2-b6e0-49797802c384)

- Using WebSocket Secure Port.   
	WebSocket Secure Port uses the WSS protocol instead of the MQTT protocol.   
	![Image](https://github.com/user-attachments/assets/b611806f-4251-4639-bfe8-13d42291baea)

__Note for using secure port.__   
The default MQTT server is ```broker.emqx.io```.   
If you use a different server, you will need to modify ```getpem.sh``` to run.   
```
chmod 777 getpem.sh
./getpem.sh
```

WebSocket/WebSocket Secure Port may differ depending on the broker used.   
If you use a different MQTT server than the default, you will need to change the port number from the default.   

__Note for using MQTTS/WS/WSS transport.__   
If you use MQTTS/WS/WSS transport, you can still publish and subscribe using MQTT transport.   
```
+----------+                   +----------+           +----------+
|          |                   |          |           |          |
|  ESP32   | ---MQTTS/WS/WSS-->|  Broker  | ---MQTT-->|Subsctiber|
|          |                   |          |           |          |
+----------+                   +----------+           +----------+

+----------+                   +----------+           +----------+
|          |                   |          |           |          |
|  ESP32   | <--MQTTS/WS/WSS---|  Broker  | <--MQTT---|Publisher |
|          |                   |          |           |          |
+----------+                   +----------+           +----------+
```



### Specifying an MQTT Broker   
You can specify your MQTT broker in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```mqtt-broker.local```   
- Fully Qualified Domain Name   
 ```broker.emqx.io```

You can use this as broker.   
https://github.com/nopnop2002/esp-idf-mqtt-broker

### Enable Secure Option
Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   
![Image](https://github.com/user-attachments/assets/6ac33531-3286-4bb7-9fa8-226e302dc489)
