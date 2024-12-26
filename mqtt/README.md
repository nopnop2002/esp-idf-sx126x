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
![config-mqtt-1](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/b7a6c61c-9d7f-4867-bae1-3f7b545b3dc9)

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


![config-mqtt-3](https://github.com/user-attachments/assets/d902cf81-716f-45d2-8d08-7be9a6da9647)

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

![config-mqtt-4](https://github.com/user-attachments/assets/3ec456da-8c5e-45dc-bd41-37d7c4b551e3)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   

### Select Transport   
This project supports TCP,SSL/TLS,WebSocket and WebSocket Secure Port.   
- Using TCP Port.   
 ![config-mqtt-tcp](https://github.com/user-attachments/assets/a22795ff-9af5-4327-bf7f-debc366dde19)

- Using SSL/TLS Port.   
 SSL/TLS Port uses the MQTTS protocol instead of the MQTT protocol.   
 ![config-mqtt-ssl](https://github.com/user-attachments/assets/7e66cda8-961d-4a48-893c-c248cd053fb5)

- Using WebSocket Port.   
 WebSocket Port uses the WS protocol instead of the MQTT protocol.   
 ![config-mqtt-ws](https://github.com/user-attachments/assets/969e7ccc-a77e-4e1b-9485-4b10531ade5e)

- Using WebSocket Secure Port.   
 WebSocket Secure Port uses the WSS protocol instead of the MQTT protocol.   
 ![config-mqtt-wss](https://github.com/user-attachments/assets/37de4b31-2ec7-49e2-8f38-2be2786dd16b)

__Note for using secure port.__   
The default MQTT server is ```broker.emqx.io```.   
If you use a different server, you will need to modify ```getpem.sh``` to run.   
```
chmod 777 getpem.sh
./getpem.sh
```

WebSocket/WebSocket Secure Port may differ depending on the broker used.   
If you use a different server, you will need to change the port number from the default.   

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

### Secure Option
Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   
![config-mqtt-5](https://github.com/user-attachments/assets/58555299-f9f0-424f-9d2f-a76b6fbe8da7)
