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


![config-mqtt-4](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/4876b215-cb75-4bea-9abb-8362b1f8910a)

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

![config-mqtt-3](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/f13f93e4-9a9b-44d0-b154-dd2059d9ced8)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   

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
