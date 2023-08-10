# WebSocket Example   
This is LoRa and WebSocket gateway application.   
```
           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
           | WS Client |-(Socket)->|   ESP32   |--(SPI)--->|  SX126x   |==(LoRa)==>
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+

           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
==(LoRa)==>|  SX126x   |--(SPI)--->|   ESP32   |-(Socket)->| WS Server |
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+
```



# Configuration
![config-top](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/7fb48cc9-d114-4481-8628-915194ead21d)
![config-app](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/0611cd8c-e36b-40b2-ab4c-51470ae9b7da)

## WiFi Setting

![config-wifi](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/cff56ea9-773d-4b28-8db6-ad2edbf9e1b4)


## Radioi Setting

### WS to LoRa
Subscribe with WebSocket and send to LoRa.   
ESP32 acts as WebSocket Server.   
You can use this script as Socket Client.   
```python3 ./ws-client.py```

```
           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
           | WS Client |-(Socket)->|   ESP32   |--(SPI)--->|  SX126x   |==(LoRa)==>
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+
```

![config-radio-1](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/c3bf9b77-f9c1-469d-87d5-e44bd1b255eb)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_RX   


### LoRa to WS
Receive from LoRa and publish as WebSocket.   
ESP32 acts as WebSocket Client.   
You can use this script as Socket Server.   
```python3 ./ws-server.py```

```
           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
==(LoRa)==>|  SX126x   |--(SPI)--->|   ESP32   |-(Socket)->| WS Server |
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+
```

![config-radio-2](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/1dabc54c-eb70-42a7-b2f8-82c1a7768c3a)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   


### Specifying an WebSocket Server   
You can specify your WebSocket Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```http-server.local```   
- Fully Qualified Domain Name   
 ```http-server.public.io```


