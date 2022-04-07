# esp-idf-sx126x
SX1262//68 Low Power Long Range Transceiver driver for esp-idf.   

I ported from [here](https://github.com/nopnop2002/Arduino-LoRa-Ra01S).

![ra01s_ra01sh](https://user-images.githubusercontent.com/6020549/161641357-a0fe292b-095e-440b-b8ae-24c58084a51d.JPG)


Ai-Thinker offers several LoRa modules.   
You can get these on AliExpress and eBay.   

|Model|Type|Interface/Core|Chip|Frequency|Foot-Pattern|IPEX-Antena|LoRa-WAN|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|Ra-01|Tranceiver|SPI|SX1278|410-525Mhz|SMD16|No|No|
|Ra-02|Tranceiver|SPI|SX1278|410-525Mhz|SMD16|Yes|No|
|Ra-01H|Tranceiver|SPI|SX1276|803-930Mhz|SMD16|No|No|
|Ra-01S|Tranceiver|SPI|**SX1268**|410-525Mhz|SMD16|No|No|
|Ra-01SH|Tranceiver|SPI|**SX1262**|803-930Mhz|SMD16|Yes|No|
|Ra-01SC|Tranceiver|SPI|LLCC68|410-525Mhz|SMD16|Yes|No|
|Ra-06|MCU|ARM Cortex M0+|SX1278|410-525Mhz|SMD20|Yes|No|
|Ra-07|MCU|ARM Cortex M0+|ASR6501|410-525Mhz|SMD18|No|Yes|
|Ra-07H|MCU|ARM Cortex M0+|ASR6501|803-930Mhz|SMD18|No|Yes|
|Ra-08|MCU|ARM Cortex M4|ASR6601|410-525Mhz|SMD18|No|Yes|
|Ra-08H|MCU|ARM Cortex M4|ASR6601|803-930Mhz|SMD18|No|Yes|


# Option with SX1262/1268
LoRa modules with SX1262/1268 have several options.   

- Using TCXO(Temperature-Compensated Crystal Oscillator)   
SX1262/1268 can use the TCXO.   
If the TCXO is used, the XTB pin is not connected.   
However, the 6th pin (DIO3) of the SX1262/1268 can be used to power the TCXO.   
Explanation for TXCO and antenna control is [here](https://github.com/beegee-tokyo/SX126x-Arduino).   
Ra-01S / Ra-01SH does not use TCXO.   

- Power supply modes   
SX1262/1268 has two power supply modes.   
One is that only LDO used in all modes.   
Another is that DC_DC+LDO used for STBY_XOSC,FS, RX and TX modes.   
Explanation for LDO and DCDC selection is [here](https://github.com/beegee-tokyo/SX126x-Arduino).   
Ra-01S / Ra-01SH use only LDO in all modes.

- RF-Switching   
In general, use DIO2 to switch the RF-Switch.   
However, some tranceiver use an external gpio to switch the RF-Switch.   
Ra-01S / Ra-01SH use the SC70-6 integrated load switch to switch between RFO and RFI.   
Ra-01S / Ra-01SH use DIO2 to control this.   
DIO2 = 1, CTRL = 0, RFC to RF1  Tx Mode.   
DIO2 = 0, CTRL = 1, RFC to R21  Rx Mode.  

You need to look at the schematic to set these options properly, but it's very esoteric.   
The default settings for this library are for Ra-01S / Ra-01SH.    
When using other than Ra-01S / Ra-01SH, you need to set them appropriately.   


# Datasheet   
- RA-01S   
https://docs.ai-thinker.com/_media/lora/docs/ra-01s_specification.pdf

- RA-01SH   
https://docs.ai-thinker.com/_media/lora/docs/ra-01sh_specification.pdf


# Foot pattern
RA-0x(SMD16) has the same foot pattern as ESP12.   
Therefore, a pitch conversion PCB for ESP12 can be used.   

![ra01s-3](https://user-images.githubusercontent.com/6020549/161641874-32a79d5f-dbae-42f1-a8cd-d0787c238a06.JPG)
![ra01s-2](https://user-images.githubusercontent.com/6020549/161641421-e720a7da-4889-4bd4-b2c6-1f3a28518cf8.JPG)


# Software requirements
esp-idf v4.4 or later.   
This is because this version supports ESP32-C3.   

# Installation

```Shell
git clone https://github.com/nopnop2002/esp-idf-sx126x
cd esp-idf-sx126x/basic/
idf.py set-target {esp32/esp32s2/esp32c3}
idf.py menuconfig
idf.py flash
```

__Note for ESP32C3__   
For some reason, there are development boards that cannot use GPIO06, GPIO08, GPIO09, GPIO19 for SPI clock pins.   
According to the ESP32C3 specifications, these pins can also be used as SPI clocks.   
I used a raw ESP-C3-13 to verify that these pins could be used as SPI clocks.   


# Configuration for Transceiver   

![config-top](https://user-images.githubusercontent.com/6020549/162125293-9405dae9-fd52-440a-b0e5-19e6b481fcf6.jpg)
![config-sx126x](https://user-images.githubusercontent.com/6020549/162125314-f0139171-3642-4e34-8013-2b6e85edd20d.jpg)

# Wirering

|SX126X||ESP32|ESP32-S2|ESP32-C3|
|:-:|:-:|:-:|:-:|:-:|
|NSS|--|GPIO15|GPIO37|GPIO7|
|RST|--|GPIO16|GPIO38|GPIO8|
|BUSY|--|GPIO17|GPIO39|GPIO9|
|TXEN|--|N/C|N/C|N/C|
|RXEN|--|N/C|N/C|N/C|
|MISO|--|GPIO19|GPIO33|GPIO18|
|MOSI|--|GPIO23|GPIO35|GPIO19|
|SCK|--|GPIO18|GPIO36|GPIO10|
|GND|--|GND|GND|GND|
|VCC|--|3.3V|3.3V|3.3V|

__You can change it to any pin using menuconfig.__   

# Reference

https://github.com/nopnop2002/esp-idf-sx127x
