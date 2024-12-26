# HTTP Example   
This is LoRa and HTTP gateway application.   
```
           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
           |HTTP Client|--(HTTP)-->|   ESP32   |--(SPI)--->|  SX126x   |==(LoRa)==>
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+

           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
==(LoRa)==>|  SX126x   |--(SPI)--->|   ESP32   |--(HTTP)-->|HTTP Server|
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+
```



# Configuration
![config-top](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/66f1c83d-499b-4db7-9a2e-67cd9396f17b)
![config-app](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/1263691f-30f9-4522-a4b9-a0f910057991)

## WiFi Setting

![config-wifi](https://github.com/nopnop2002/esp-idf-sx126x/assets/6020549/6560cc80-1a81-4537-8c8a-ffc527ee97fe)


## Radio Setting

### HTTP to LoRa
Subscribe with HTTP and send to LoRa.   
ESP32 acts as HTTP Server.   
You can use curl as HTTP Client.   
```sh ./http-client.sh```

```
           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
           |HTTP Client|--(HTTP)-->|   ESP32   |--(SPI)--->|  SX126x   |==(LoRa)==>
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+
```

![config-radio-1](https://github.com/user-attachments/assets/7b12cf37-e972-43c0-add0-ccf7c756b74d)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_RX   


### LoRa to HTTP
Receive from LoRa and publish as HTTP.   
ESP32 acts as HTTP Client.   
You can use nc(netcat) as HTTP Server.   
```sh ./http-server.sh```

```
           +-----------+           +-----------+           +-----------+
           |           |           |           |           |           |
==(LoRa)==>|  SX126x   |--(SPI)--->|   ESP32   |--(HTTP)-->|HTTP Server|
           |           |           |           |           |           |
           +-----------+           +-----------+           +-----------+
```

![config-radio-2](https://github.com/user-attachments/assets/6f5d2e94-f11c-403e-9b45-ebff1f146421)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   


### Specifying an HTTP Server   
You can specify your HTTP Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```http-server.local```   
- Fully Qualified Domain Name   
 ```http-server.public.io```


