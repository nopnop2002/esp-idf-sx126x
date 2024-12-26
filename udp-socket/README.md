# UDP-SOCKET Example   
This is LoRa and UDP-SOCKET gateway application.   
```
           +------------+           +------------+           +------------+
           |            |           |            |           |            |
           | UDP Client |-(UDP/IP)->|    ESP32   |--(SPI)--->|   SX126x   |==(LoRa)==>
           |            |           |            |           |            |
           +------------+           +------------+           +------------+

           +------------+           +------------+           +------------+
           |            |           |            |           |            |
==(LoRa)==>|   SX126x   |--(SPI)--->|    ESP32   |-(UDP/IP)->| UDP Server |
           |            |           |            |           |            |
           +------------+           +------------+           +------------+
```



# Configuration
![config-top](https://github.com/user-attachments/assets/d6e9f71b-471d-4e63-9c2d-d3eeeab0c7a2)
![config-app](https://github.com/user-attachments/assets/8fa7cf71-be12-4b00-a2ab-0e566be66d54)

## WiFi Setting

![config-wifi](https://github.com/user-attachments/assets/2f8f4437-9fa5-4c67-8604-8c5ad1069db6)

## Radio Setting

### UDP to LoRa
Subscribe with UDP and send to LoRa.   
ESP32 acts as UDP Server.   
You can use this script as UDP Client.   
```python3 ./udp-client.py```

```
           +------------+           +------------+           +------------+
           |            |           |            |           |            |
           | UDP Client |-(UDP/IP)->|    ESP32   |--(SPI)--->|   SX126x   |==(LoRa)==>
           |            |           |            |           |            |
           +------------+           +------------+           +------------+
```

![config-radio-1](https://github.com/user-attachments/assets/f4c3319f-2460-43a1-9a75-be1f366084c7)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_RX   


### LoRa to UDP
Receive from LoRa and publish as UDP.   
ESP32 acts as UDP Client.   
You can use this script as UDP Server.   
```python3 ./udp-server.py```

```
           +------------+           +------------+           +------------+
           |            |           |            |           |            |
==(LoRa)==>|   SX126x   |--(SPI)--->|    ESP32   |-(UDP/IP)->| UDP Server |
           |            |           |            |           |            |
           +------------+           +------------+           +------------+
```

![config-radio-2](https://github.com/user-attachments/assets/2d902dd0-494e-4e41-9385-45519f1b834b)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   


### Specifying an UDP Address   
There are the following four methods for specifying the UDP Address.   

- Limited broadcast address   
 The address represented by 255.255.255.255, or \<broadcast\>, cannot cross the router.   
 Both the sender and receiver must specify a Limited broadcast address.   

- Directed broadcast address   
 It is possible to cross the router with an address that represents only the last octet as 255, such as 192.168.10.255.   
 Both the sender and receiver must specify the Directed broadcast address.   
 __Note that it is possible to pass through the router.__   

- Multicast address   
 Data is sent to all PCs belonging to a specific group using a special address (224.0.0.0 to 239.255.255.255) called a multicast address.   
 I've never used it, so I don't know anything more.

- Unicast address   
 It is possible to cross the router with an address that specifies all octets, such as 192.168.10.41.   
 Both the sender and receiver must specify the Unicast address.



