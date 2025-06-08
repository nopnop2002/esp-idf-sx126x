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
![Image](https://github.com/user-attachments/assets/ad2cf3fc-447e-4744-b238-5cbd1fe07a91)
![Image](https://github.com/user-attachments/assets/8c8ecc11-4f60-466c-996b-ac085c69d924)

## WiFi Setting
Set the information of your access point.
![Image](https://github.com/user-attachments/assets/d2c0a00b-5d33-4828-9ffc-920c32298f5d)

## Radio Setting
Set the wireless communication direction.
![Image](https://github.com/user-attachments/assets/51f1b34c-51c1-4164-8244-b1b989cc13f0)

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


Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   

## Broker Setting
Set the information of your MQTT broker.
![Image](https://github.com/user-attachments/assets/e49991a2-6fcf-4ea5-9de2-fcac5d2bfe23)

### Select Transport   
This project supports TCP,SSL/TLS,WebSocket and WebSocket Secure Port.   

- Using TCP Port.   
	TCP Port uses the MQTT protocol.   

- Using SSL/TLS Port.   
	SSL/TLS Port uses the MQTTS protocol instead of the MQTT protocol.   

- Using WebSocket Port.   
	WebSocket Port uses the WS protocol instead of the MQTT protocol.   

- Using WebSocket Secure Port.   
	WebSocket Secure Port uses the WSS protocol instead of the MQTT protocol.   

__Note for using secure port.__   
The default MQTT server is ```broker.emqx.io```.   
If you use a different server, you will need to modify ```getpem.sh``` to run.   
```
chmod 777 getpem.sh
./getpem.sh
```

WebSocket/WebSocket Secure Port may differ depending on the broker used.   
If you use a different MQTT server than the default, you will need to change the port number from the default.   

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

### Select MQTT Protocol   
This project supports MQTT Protocol V3.1.1/V5.   
![Image](https://github.com/user-attachments/assets/23141633-44c3-41f0-92f9-f7d13d254908)

### Enable Secure Option   
Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   
![Image](https://github.com/user-attachments/assets/f340b7f2-25b4-4edc-8f6e-2bb4dca183fb)
