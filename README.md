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
__When using other than Ra-01S / Ra-01SH, you need to set them appropriately.__   
I don't have a SX126X other than Ra-01S / Ra-01SH.   
Therefore, I don't know the settings of SX126X other than Ra-01S / Ra-01SH.   

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
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3}
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

|SX126X||ESP32|ESP32-S2/S3|ESP32-C3|
|:-:|:-:|:-:|:-:|:-:|
|BUSY|--|GPIO17|GPIO39|GPIO9|
|RST|--|GPIO16|GPIO38|GPIO8|
|TXEN|--|N/C|N/C|N/C|
|RXEN|--|N/C|N/C|N/C|
|MISO|--|GPIO19|GPIO37|GPIO18|
|SCK|--|GPIO18|GPIO36|GPIO10|
|MOSI|--|GPIO23|GPIO35|GPIO19|
|NSS|--|GPIO15|GPIO34|GPIO7|
|GND|--|GND|GND|GND|
|VCC|--|3.3V|3.3V|3.3V|

__You can change it to any pin using menuconfig.__   

# Limitation
- The SX126x chip implements FSK, but FSK is not supported in this library.   
- Interrupts is not supported in this library.   


# SX1262 and SX1278, SX1276 Comparison
[This](https://www.ebyte.com/en/new-view-info.aspx?id=303) will be helpful.   


# Trouble shooting   
If it doesn't look like this at boot time, the wirering is incorrect.   
```
I (3425) RA01S: syncWord=0x1424
I (3425) RA01S: SX126x installed
```
If that doesn't work, enable [LoRaDebugPrint](https://github.com/nopnop2002/esp-idf-sx126x/blob/main/basic/main/main.c#L123).   
This is my logging.   
```
I (0) cpu_start: Starting scheduler on APP CPU.
I (332) main: Frequency is 433MHz
I (332) RA01S: CONFIG_MISO_GPIO=19
I (342) RA01S: CONFIG_MOSI_GPIO=23
I (342) RA01S: CONFIG_SCLK_GPIO=18
I (352) RA01S: CONFIG_NSS_GPIO=15
I (352) RA01S: CONFIG_RST_GPIO=16
I (352) RA01S: CONFIG_BUSY_GPIO=17
I (362) RA01S: CONFIG_TXEN_GPIO=-1
I (362) RA01S: CONFIG_RXEN_GPIO=-1
I (372) gpio: GPIO[15]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (382) gpio: GPIO[16]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (392) gpio: GPIO[17]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (392) RA01S: spi_bus_initialize=0
I (402) RA01S: spi_bus_add_device=0
I (442) RA01S: Reset
I (442) RA01S: ReadRegister: REG=0x740
I (442) RA01S: DataIn:14
I (442) RA01S: DataIn:24
I (452) RA01S: syncWord=0x1424
I (452) RA01S: SX126x installed
I (452) RA01S: WriteCommand: CMD=0x80
I (462) RA01S: 00 --> a2
I (462) RA01S: WriteCommand: CMD=0x9d
I (472) RA01S: 01 --> a2
I (472) RA01S: tcxoVoltage=0.000000
I (472) RA01S: WriteCommand: CMD=0x89
I (482) RA01S: 7f --> a2
I (482) RA01S: useRegulatorLDO=0
I (482) RA01S: WriteCommand: CMD=0x96
I (492) RA01S: 01 --> a2
I (492) RA01S: WriteCommand: CMD=0x8f
I (502) RA01S: 00 --> a2
I (502) RA01S: 00 --> a2
I (502) RA01S: WriteCommand: CMD=0x95
I (512) RA01S: 04 --> a2
I (512) RA01S: 07 --> a2
I (512) RA01S: 00 --> a2
I (522) RA01S: 01 --> a2
I (522) RA01S: WriteRegister: REG=0x8e7
I (522) RA01S: 18 --> a2
I (532) RA01S: WriteCommand: CMD=0x8e
I (532) RA01S: 16 --> a2
I (532) RA01S: 04 --> a2
I (542) RA01S: WriteCommand: CMD=0x98
I (542) RA01S: 6b --> a2
I (552) RA01S: 6f --> a2
I (552) RA01S: WriteCommand: CMD=0x86
I (552) RA01S: 1b --> a2
I (562) RA01S: 10 --> a2
I (562) RA01S: 00 --> a2
I (562) RA01S: 00 --> a2
I (572) MAIN: LoRaBegin=0
I (572) RA01S: WriteCommand: CMD=0x9f
I (572) RA01S: 00 --> a2
I (582) RA01S: WriteCommand: CMD=0xa0
I (582) RA01S: 00 --> a2
I (582) RA01S: WriteCommand: CMD=0x8a
I (592) RA01S: 01 --> a2
I (592) RA01S: WriteCommand: CMD=0x8b
I (602) RA01S: 07 --> a2
I (602) RA01S: 04 --> a2
I (602) RA01S: 01 --> a2
I (612) RA01S: 00 --> a2
I (612) RA01S: ReadRegister: REG=0x736
I (612) RA01S: DataIn:0d
I (622) RA01S: WriteRegister: REG=0x736
I (622) RA01S: 09 --> a2
I (622) RA01S: WriteCommand: CMD=0x8c
I (632) RA01S: 00 --> a2
I (632) RA01S: 08 --> a2
I (642) RA01S: 00 --> a2
I (642) RA01S: ff --> a2
I (642) RA01S: 01 --> a2
I (652) RA01S: 00 --> a2
I (652) RA01S: WriteCommand: CMD=0x08
I (652) RA01S: 03 --> a2
I (662) RA01S: ff --> a2
I (662) RA01S: 00 --> a2
I (662) RA01S: 00 --> a2
I (672) RA01S: 00 --> a2
I (672) RA01S: 00 --> a2
I (672) RA01S: 00 --> a2
I (682) RA01S: 00 --> a2
I (682) RA01S: ----- SetRx timeout=16777215
I (682) RA01S: WriteCommand: CMD=0x80
I (692) RA01S: 00 --> a2
I (692) RA01S: SetRxEnable:SX126x_TXEN=-1 SX126x_RXEN=-1
I (702) RA01S: WriteCommand: CMD=0x82
I (702) RA01S: ff --> a2
I (702) RA01S: ff --> a2
I (712) RA01S: ff --> a2
I (712) RA01S: ReadCommand: CMD=0xc0
I (712) RA01S: DataIn:52
I (722) RA01S: ReadCommand: CMD=0xc0
I (722) RA01S: DataIn:52
I (732) task_tx: Start
```

# Reference   

https://github.com/nopnop2002/esp-idf-sx127x
