# Ping-Pong Example   
Send data from primary to secondary.   
In the secondary, the characters are converted and sent back.   

- ESP32 is Primary   
When using Arduino, use Ra01S-Pong.   
```
+-----------+           +-----------+            +-----------+           +-----------+
|           |           |           |            |           |           |           |
|  Primary  |===(SPI)==>|  SX126x   |---(LoRa)-->|  SX126x   |===(SPI)==>| Secondary |
|   ESP32   |           |           |            |           |           |           |
|           |           |           |            |           |           |           |
|           |<==(SPI)===|           |<--(LoRa)---|           |<==(SPI)===|           |
|           |           |           |            |           |           |           |
+-----------+           +-----------+            +-----------+           +-----------+
```

- ESP32 is Secondary   
When using Arduino, use Ra01S-Ping.   

```
+-----------+           +-----------+            +-----------+           +-----------+
|           |           |           |            |           |           |           |
|  Primary  |===(SPI)==>|  SX126x   |---(LoRa)-->|  SX126x   |===(SPI)==>| Secondary |
|           |           |           |            |           |           |   ESP32   |
|           |           |           |            |           |           |           |
|           |<==(SPI)===|           |<--(LoRa)---|           |<==(SPI)===|           |
|           |           |           |            |           |           |           |
+-----------+           +-----------+            +-----------+           +-----------+
```

# Configuration   
![config-app](https://user-images.githubusercontent.com/6020549/162128875-d5acb0fb-808a-4b1b-949f-81a163213636.jpg)
![config-ping-pong-1](https://github.com/user-attachments/assets/18961170-c5f9-40d5-81af-ea8468f0b51a)


# Response time   
Time (Ticks) required from Start to Finish.   
1 Tick is 10 milliseconds.   
```
+-----------+           +-----------+            +-----------+           +-----------+
|           |Start      |           |            |           |           |           |
|  Primary  |===(SPI)==>|  SX126x   |---(LoRa)-->|  SX126x   |===(SPI)==>| Secondary |
|   ESP32   |           |           |            |           |           |   ESP32   |
|           |Finish     |           |            |           |           |           |
|           |<==(SPI)===|           |<--(LoRa)---|           |<==(SPI)===|           |
|           |           |           |            |           |           |           |
+-----------+           +-----------+            +-----------+           +-----------+
```

### With varying Signal Bandwidth(BW)   
ESP32@160/433MHz/CR=1/SF=7   
|Payload|BW=0(7.8KHz)|BW=1(15.6KHz)|BW=2(31.25KHz)|BW=3(62.5KHz)|BW=4(125KHz)|BW=5(250KHz)|BW=6(500KHz)|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|32Bytes|241|125|65|37|22|14|*2|
|64Bytes|*1|199|102|56|32|20|14|
|128Bytes|*1|*1|181|94|52|30|18|
|255Bytes|*1|*1|328|169|89|48|28|

(*1)The sent data and the response data do not match.   

(*2) SetTx Illegal Status occurs. Cause unknown.   


### With varying Error coding rate(CR)   
ESP32@160/433MHz/BW=4/SF=7   
|Payload|CR=1(4/5)|CR=2(4/6)|CR=3(4/7)|CR=4(4/8)|
|:-:|:-:|:-:|:-:|:-:|
|32Bytes|22|23|27|29|
|64Bytes|32|36|38|43|
|128Bytes|52|59|67|74|
|255Bytes|89|103|119|133|
