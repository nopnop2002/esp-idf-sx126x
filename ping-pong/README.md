# Ping-Pong Example   
Send data from primary to secondary.   
In the secondary, the characters are converted and sent back.   

- ESP32 is Primary   
When using Arduino, use Ra01S-Pong.   
```
+-----------+            +-----------+           +-----------+
|           |            |           |           |           |
| Secondary |<==(LoRa)===|  SX126x   |<--(SPI)---|   ESP32   |
|           |            |           |           |  Primary  |
|           |            |           |           |           |
|           |===(LoRa)==>|           |---(SPI)-->|           |
|           |            |           |           |           |
+-----------+            +-----------+           +-----------+
```

- ESP32 is Secondary   
When using Arduino, use Ra01S-Ping.   

```
+-----------+            +-----------+           +-----------+
|           |            |           |           |           |
|  Primary  |===(LoRa)==>|  SX126x   |---(SPI)-->|   ESP32   |
|           |            |           |           | Secondary |
|           |            |           |           |           |
|           |<==(LoRa)===|           |<--(SPI)---|           |
|           |            |           |           |           |
+-----------+            +-----------+           +-----------+
```

# Configuration   
![config-app](https://user-images.githubusercontent.com/6020549/162128875-d5acb0fb-808a-4b1b-949f-81a163213636.jpg)
![config-ping-pong-1](https://github.com/user-attachments/assets/18961170-c5f9-40d5-81af-ea8468f0b51a)


