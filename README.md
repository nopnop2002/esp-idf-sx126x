# esp-idf-sx126x
SX1262/SX1268/LLCC68 Low Power Long Range Transceiver driver for esp-idf.   

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
|Ra-01SC|Tranceiver|SPI|**LLCC68**|410-525Mhz|SMD16|Yes|No|
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
DIO2 = 1, CTRL = 0, RFC for SC70-6 goes to RF1 -->  Switch to Tx Mode.   
DIO2 = 0, CTRL = 1, RFC for SC70-6 gose to RF2 -->  Switch to Rx Mode.   

You need to look at the schematic to set these options properly, but it's very esoteric.   
The default settings for this library are for Ra-01S / Ra-01SH.    
__When using other than Ra-01S / Ra-01SH, you need to set them appropriately.__   

# About Ra-01SC   
Ra-01SC uses LLCC68.   
Ra-01SC is compatible with Ra-01S.   
However, there are the following restrictions:   
- BW is either 125KHz, 250KHz or 500Khz.   
- When BW is 125KHz, SF is in the range of 5-9.   
- When BW is 250KHz, SF is in the range of 5-10.   
- When BW is 500KHz, SF is in the range of 5-11.   

__When using LLCC68 other than Ra-01SC, it is necessary to set the options appropriately based on the datasheet.__   

![ra01sc](https://user-images.githubusercontent.com/6020549/169180199-12a88938-1d6d-43c7-836d-f3c2081ac10d.JPG)

# Datasheet   
- RA-01S   
https://docs.ai-thinker.com/_media/lora/docs/ra-01s_specification.pdf

- RA-01SH   
https://docs.ai-thinker.com/_media/lora/docs/ra-01sh_specification.pdf

- RA-01SC(Chinese)   
https://img.iceasy.com/product/product/files/202108/8a8a8a1a7aec7b55017b2ef70a370953.pdf

# Foot pattern
RA-0x(SMD16) has the same foot pattern as ESP12.   
Therefore, a pitch conversion PCB for ESP12 can be used.   
We have pitch conversion PCBs available on eBay and AliExpress.   

![ra01s-3](https://user-images.githubusercontent.com/6020549/161641874-32a79d5f-dbae-42f1-a8cd-d0787c238a06.JPG)
![ra01s-2](https://user-images.githubusercontent.com/6020549/161641421-e720a7da-4889-4bd4-b2c6-1f3a28518cf8.JPG)


# Software requirements
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   

# Installation

```Shell
git clone https://github.com/nopnop2002/esp-idf-sx126x
cd esp-idf-sx126x/basic/
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```

__Note for ESP32C3__   
For some reason, there are development boards that cannot use GPIO06, GPIO08, GPIO09, GPIO19 for SPI clock pins.   
According to the ESP32C3 specifications, these pins can also be used as SPI clocks.   
I used a raw ESP-C3-13 to verify that these pins could be used as SPI clocks.   


# Configuration for Transceiver   

![config-top](https://user-images.githubusercontent.com/6020549/162125293-9405dae9-fd52-440a-b0e5-19e6b481fcf6.jpg)

## Frequency used   
![config-sx126x-1](https://github.com/user-attachments/assets/4618931b-a0ad-4db4-b870-34d59adc876a)

## Using a transceiver other than 433MHz / 866MHz / 915MHz   
![config-sx126x-2](https://github.com/user-attachments/assets/f6a52627-39fd-4c96-ade9-2a7f75bba06b)

## advanced settings   
![config-sx126x-3](https://github.com/user-attachments/assets/d498e31f-a593-4e63-807a-115bd8b7fe43)

LoRa mode has the following three communication parameters.   
1.Error Coding Rate (= CR)   
2.Signal Bandwidth (= BW)   
3.Spreading Factor (= SF)   
The communication speed is faster when BW is large, CR is small, and SF is small.   
However, as the communication speed increases, the reception sensitivity deteriorates, so select the one that best suits your needs.   

- Error Coding Rate   
1:4/5(Default)   
2:4/6   
3:4/7   
4:4/8   

- Signal Bandwidth   
0:7.81 kHz   
1:15.63 kHz   
2:31.25 kHz   
3:62.50 kHz   
4:125 kHz(Default)   
5:250 kHz   
6:500 kHz   

- Spreading Factor (expressed as a base-2 logarithm)   
5:32 chips / symbol   
6:64 chips / symbol   
7:128 chips / symbol(Default)   
8:256 chips / symbol   
9:512 chips / symbol   
10:1024 chips / symbol   
11:2048 chips / symbol   
12:4096 chips / symbol   

## SPI BUS selection   
![config-sx126x-4](https://github.com/user-attachments/assets/6a60fdff-5e70-4483-89a9-a1394992ba2e)

The ESP32 series has three SPI BUSs.   
SPI1_HOST is used for communication with Flash memory.   
You can use SPI2_HOST and SPI3_HOST freely.   
When you use SDSPI(SD Card via SPI), SDSPI uses SPI2_HOST BUS.   
When using this module at the same time as SDSPI or other SPI device using SPI2_HOST, it needs to be changed to SPI3_HOST.   
When you don't use SDSPI, both SPI2_HOST and SPI3_HOST will work.   
Previously it was called HSPI_HOST / VSPI_HOST, but now it is called SPI2_HOST / SPI3_HOST.   

# Wirering

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

__You can change it to any pin using menuconfig.__   

# Using EBYTE Module

EBYTE offers several LoRa modules.   
You can get these on AliExpress and eBay.   

|Model|Interface|Chip|Frequency|Power|Foot-Patten|IPEX-Antena|LoRa-WAN|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|E22-400M22S|SPI|SX1268|433/470Mhz|160mW|Standard|Yes|No|
|E22-400M30S|SPI|SX1268|433/470Mhz|1000mW|Standard|Yes|No|
|E22-400MM22S|SPI|SX1268|433/470Mhz|160mW|Small|No|No|
|E22-900M22S|SPI|SX1262|868/915Mhz|160mW|Standard|Yes|No|
|E22-900M30S|SPI|SX1262|868/915Mhz|1000mW|Standard|Yes|No|
|E22-900MM22S|SPI|SX1262|868/915Mhz|160mW|Small|No|No|

![EBYTE-1](https://user-images.githubusercontent.com/6020549/221332908-b3796a81-8cb0-4649-b0a8-d809b4172d3c.JPG)
![EBYTE-2](https://user-images.githubusercontent.com/6020549/221332911-92f45aed-0c4e-4623-b0e7-92575ec0d0a9.JPG)

With this change it work.   
![config-ebyte](https://github.com/user-attachments/assets/69ab8000-c6c0-4de5-8cdf-74308ec3edc5)

Two additional wires are required.   
|SX126X||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6|
|:-:|:-:|:-:|:-:|:-:|
|TXEN|--|5|5|1|
|RXEN|--|4|4|0|

The pitch conversion base is [here](https://github.com/nopnop2002/esp-idf-sx126x/tree/main/ebyte-smd-pcb).   

SX1262 and LLCC68 are compatible, but the following modules do not work for some reason.
|Model|Interface|Chip|Frequency|Power|Foot-Patten|IPEX-Antena|LoRa-WAN|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|E220-400M22S|SPI|LLCC68|433/470Mhz|160mW|Standard|Yes|No|
|E220-400M30S|SPI|LLCC68|433/470Mhz|1000mW|Standard|Yes|No|
|E220-900M22S|SPI|LLCC68|868/915Mhz|160mW|Standard|Yes|No|
|E220-900M30S|SPI|LLCC68|868/915Mhz|1000mW|Standard|Yes|No|

# Using Heltec LoRa V3
Works with the same changes as EBYTE Module.   

# Limitation
- The SX126x chip implements FSK, but FSK is not supported in this library.   
- Interrupts is not supported in this library.   

# Error Handling   
If an error occurs within the library, this library calls the following function:   
```
void LoRaError(int error);
```

This function is defined as a weak function, so you can change it to whatever you like.   
```
void LoRaError(int error)
{
    # Place your favorite code
    ESP_LOGE(TAG, "LoRaError=%d", error);
    while (true) {
        vTaskDelay(1);
    }
}
```


# Trouble shooting   
If it doesn't look like this at boot time, the wirering is incorrect.   
```
I (3425) RA01S: syncWord=0x1424
I (3425) RA01S: SX126x installed
```

If that doesn't work, enable [LoRaDebugPrint](https://github.com/nopnop2002/esp-idf-sx126x/blob/main/basic/main/main.c#L101).   
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

If this error occurs, check the antenna.   
```
E (14993) RA01S: SetTx Illegal Status
```

# Communication with SX127X
LoRa's packet format is strictly specified.   
Therefore, if the following three parameters are the same, they can communicate with each other.   
- Signal Bandwidth (= BW)   
- Error Cording Rate (= CR)   
- Spreading Factor (= SF)   

# About communication speed and maximum reception sensitivity   
In LoRa modulation mode, the communication speed (bps) and maximum reception sensitivity (RSSI) are determined by a combination of spreading factor (SF), bandwidth (BW), and coding rate (CDR).   
- SF   
Increasing SF increases the spreading factor and improves noise resistance.   
This improves reception sensitivity and extends communication distance, but communication speed decreases.   
- BW   
Bandwidth sets the width of the communication band. Setting a larger bandwidth will improve communication speed.   
However, the radio reception sensitivity (RSSI) will decrease.   
- CDR   
CDR (CodingRate) sets the level of error correction rate.   
The larger the number, the better the correction rate, but the amount of information per packet increases.   
(No effect on maximum reception sensitivity)   
You can set whether to use Optimaise for each CDR, and enabling it will improve the correction rate, but will reduce communication speed.   

# Throughput with varying Signal Bandwidth(BW) (Unit=Bytes/Sec)   
ESP32@160/433MHz/CR=1/SF=7   
|BW=0(7.81KHz)|BW=1(15.63KHz)|BW=2(31.25KHz)|BW=3(62.5KHz)|BW=4(125KHz)|BW=5(250KHz)|BW=6(500KHz)|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|41.32|81.85|160.73|309.65|578.89|1022.04|1650.49|

# Throughput with varying Error coding rate(CR) (Unit=Bytes/Sec)   
ESP32@160/433MHz/BW=7/SF=7   
|CR=1(4/5)|CR=2(4/6)|CR=3(4/7)|CR=4(4/8)|
|:-:|:-:|:-:|:-:|
|578.89|493.71|429.65|386.07|

# SX1262 and SX1278, SX1276 Comparison
[This](https://www.cdebyte.com/news/580) will be helpful.   


# Reference   

https://github.com/nopnop2002/esp-idf-sx127x
