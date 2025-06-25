# TCP-SOCKET Example   
This is LoRa and TCP-SOCKET gateway application.   
```
           +------------+           +------------+           +------------+
           |            |           |            |           |            |
           | TCP Client |-(TCP/IP)->|    ESP32   |--(SPI)--->|   SX126x   |==(LoRa)==>
           |            |           |            |           |            |
           +------------+           +------------+           +------------+

           +------------+           +------------+           +------------+
           |            |           |            |           |            |
==(LoRa)==>|   SX126x   |--(SPI)--->|    ESP32   |-(TCP/IP)->| TCP Server |
           |            |           |            |           |            |
           +------------+           +------------+           +------------+
```



# Configuration
![config-top](https://github.com/user-attachments/assets/519b466f-3884-4d02-bcc0-aef6568d7b45)
![config-app](https://github.com/user-attachments/assets/9af9ab0a-2f30-409c-a1fe-0503b8a3b12e)

## WiFi Setting
Set the information of your access point.   
![config-wifi](https://github.com/user-attachments/assets/b2698dc6-ad2e-4fc5-84a4-897a2bcaa389)

## Radio Setting
Set the wireless communication direction.   

### TCP to LoRa
Receive from TCP and send to LoRa.   
ESP32 acts as TCP Server.   
You can use this script as TCP Client.   
```python3 ./tcp-client.py```

```
           +------------+           +------------+           +------------+
           |            |           |            |           |            |
           | TCP Client |-(TCP/IP)->|    ESP32   |--(SPI)--->|   SX126x   |==(LoRa)==>
           |            |           |            |           |            |
           +------------+           +------------+           +------------+
```

![config-radio-1](https://github.com/user-attachments/assets/3f17d630-2587-4736-b43f-3973b06428f2)

Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-sx126x/tree/main/ArduinoCode/Ra01S-RX).   

### LoRa to TCP
Receive from LoRa and send to TCP.   
ESP32 acts as TCP Client.   
You can use this script as TCP Server.   
```python3 ./tcp-server.py```

```
           +------------+           +------------+           +------------+
           |            |           |            |           |            |
==(LoRa)==>|   SX126x   |--(SPI)--->|    ESP32   |-(TCP/IP)->| TCP Server |
           |            |           |            |           |            |
           +------------+           +------------+           +------------+
```

![config-radio-2](https://github.com/user-attachments/assets/647012f5-3a67-47fe-9dca-008d01a005f7)

Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-sx126x/tree/main/ArduinoCode/Ra01S-TX).   

### Specifying an TCP Server   
You can specify your TCP Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```tcp-server.local```   


