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

![config-wifi](https://github.com/user-attachments/assets/b2698dc6-ad2e-4fc5-84a4-897a2bcaa389)

## Radio Setting

### TCP to LoRa
Subscribe with TCP and send to LoRa.   
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

![config-radio-1](https://github.com/user-attachments/assets/ecb584a7-bd22-4312-884f-0b6b2bfe4a38)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_RX   


### LoRa to TCP
Receive from LoRa and publish as TCP.   
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

![config-radio-2](https://github.com/user-attachments/assets/346a2f00-bec1-49b7-9b9c-654d67befd64)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   


### Specifying an TCP Server   
You can specify your TCP Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```tcp-server.local```   


