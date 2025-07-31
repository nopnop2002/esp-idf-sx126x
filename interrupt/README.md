# Interrupt Example   
The SX126x supports the following interrupts.   
This project demonstrates how to use TxDone or RxDone.   
__Bit 3 cannot be used with LoRa.__   

|Bit|IRQ|Description|Modulation|
|:-:|:-:|:-:|:-:|
|0|TxDone|Packet transmission completed|All|
|1|RxDone|Packet received|All|
|2|PreambleDetected|Preamble detected|All|
|3|SyncWordValid|Valid sync word detected|FSK|
|4|HeaderValid|Valid LoRa header received|LoRa|
|5|HeaderErr|LoRa header CRC error|LoRa|
|6|CrcErr|Wrong CRC received|All|
|7|CadDone|Channel activity detection finished|LoRa|
|8|CadDetected|Channel activity detected|LoRa|
|9|Timeout|Rx or Tx timeout|All|


# Configuration   
![Image](https://github.com/user-attachments/assets/381a395e-e04f-4fdd-9579-f8353d44245b)
![Image](https://github.com/user-attachments/assets/292df250-48d8-489c-9beb-1ecaba7fc37c)

# Wirering
Add a wire to DIO1.   
|SX126X||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6|
|:-:|:-:|:-:|:-:|:-:|
|BUSY|--|GPIO17|GPIO39|GPIO3|
|RST|--|GPIO16|GPIO38|GPIO2|
|TXEN|--|N/C|N/C|N/C|
|RXEN|--|N/C|N/C|N/C|
|MISO|--|GPIO19|GPIO37|GPIO4|
|SCK|--|GPIO18|GPIO36|GPIO5|
|MOSI|--|GPIO23|GPIO35|GPIO6|
|NSS|--|GPIO15|GPIO34|GPIO7|
|GND|--|GND|GND|GND|
|VCC|--|3.3V|3.3V|3.3V|
|DIO1|--|GPIO21|GPIO1|GPIO1|

# How to use interrupt
irqMask sets the interrupt bits you need. You can set multiple bits.   
dioXMask sets the interrupt bits to notify DIOX. You can set multiple bits.   
The following setting enables all interrupts:   
When SX126X_IRQ_TX_DONE occurs, DIO1 turns ON.   
When SX126X_IRQ_RX_DONE occurs, DIO2 turns ON.   
When SX126X_IRQ_TIMEOUT occurs, DIO3 turns ON.   
```
    uint16_t irqMask = SX126X_IRQ_ALL;
    uint16_t dio1Mask = SX126X_IRQ_TX_DONE;
    uint16_t dio2Mask = SX126X_IRQ_RX_DONE;;
    uint16_t dio3Mask = SX126X_IRQ_TIMEOUT;
    SetDioIrqParams(irqMask, dio1Mask, dio2Mask, dio3Mask);
```

This will give the same result as above.   
```
    uint16_t irqMask = SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT;
    uint16_t dio1Mask = SX126X_IRQ_TX_DONE;
    uint16_t dio2Mask = SX126X_IRQ_RX_DONE;
    uint16_t dio3Mask = SX126X_IRQ_TIMEOUT;
    SetDioIrqParams(irqMask, dio1Mask, dio2Mask, dio3Mask);
```
