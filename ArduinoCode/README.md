# Example of Arduino environment   
I use [this](https://github.com/nopnop2002/Arduino-LoRa-Ra01S).   

# Wireing
|Ra-01S/SH||UNO|MEG|ESP8266|
|:-:|:-:|:-:|:-:|:-:|
|VCC|--|3.3V(*1)|3.3V|3.3V|
|GND|--|GND|GND|GND|
|SCK|--|D13(*2)|D52(*2)|IO14|
|MISO|--|D12|D50|IO12|
|MOSI|--|D11(*2)|D51(*2)|IO13|
|NSS|--|D5(*2)|D5(*2)|IO2|
|RST|--|D6(*2)|D6(*2)|IO0|
|BUSY|--|D7(*2)|D7(*2)|IO16|

(*1)   
UNO's 3.3V output can only supply 50mA.   
In addition, the output current capacity of UNO-compatible devices is smaller than that of official products.   
__So this module may not work normally when supplied from the on-board 3v3.__   

(*2)   
SX126x is not 5V tolerant.   
You need level shift from 5V to 3.3V.   
I used [this](https://www.ti.com/lit/ds/symlink/txs0108e.pdf?ts=1647593549503) for a level shift.   

