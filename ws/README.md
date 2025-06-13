# WebSocket Example   
This is LoRa and WebSocket gateway application.   
```
           +-----------+              +-----------+              +-----------+
           |           |              |           |              |           |
           | Web Client|-(WebSocket)->|   ESP32   |----(SPI)---->|  SX126x   |==(LoRa)==>
           |           |              |           |              |           |
           +-----------+              +-----------+              +-----------+

           +-----------+              +-----------+              +-----------+
           |           |              |           |              |           |
==(LoRa)==>|  SX126x   |----(SPI)---->|   ESP32   |-(WebSocket)->| Web Server|
           |           |              |           |              |           |
           +-----------+              +-----------+              +-----------+
```



# Configuration
![config-top](https://github.com/user-attachments/assets/1d37bde1-4577-49f4-bee6-e790ab02293f)
![config-app](https://github.com/user-attachments/assets/6657f933-c038-46e3-9991-a1ce2c7ff7eb)

## WiFi Setting
Set the information of your access point.   
![config-wifi](https://github.com/user-attachments/assets/9d573857-b6f3-4c13-aa20-1812c97d6fb9)

## Radio Setting
Set the wireless communication direction.   

### WebSocket to LoRa
Subscribe with WebSocket and send to LoRa.   
ESP32 acts as Web Server.   
You can use ws-client.py as Web Client.   
You need to install websockets from here.   
https://github.com/python-websockets/websockets   

```
           +-----------+              +-----------+              +-----------+
           |           |              |           |              |           |
           | Web Client|-(WebSocket)->|   ESP32   |----(SPI)---->|  SX126x   |==(LoRa)==>
           |           |              |           |              |           |
           +-----------+              +-----------+              +-----------+
```

![config-radio-1](https://github.com/user-attachments/assets/24984d01-ebb6-4e7a-81b6-96e28c8e9c9f)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\LoRaReceiver   


### LoRa to WebSocket
Receive from LoRa and publish as WebSocket.   
ESP32 acts as Web Client.   
You can use ws-server.py as Web Server.   
You need to install websocket-server from here.   
https://github.com/Pithikos/python-websocket-server   

```
           +-----------+              +-----------+              +-----------+
           |           |              |           |              |           |
==(LoRa)==>|  SX126x   |----(SPI)---->|   ESP32   |-(WebSocket)->| Web Server|
           |           |              |           |              |           |
           +-----------+              +-----------+              +-----------+
```

![config-radio-2](https://github.com/user-attachments/assets/aff44b02-538a-4cf8-baef-c0ae4fde1341)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\LoRaSender   


### Specifying an WebSocket Server   
You can specify your WebSocket Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```http-server.local```   


