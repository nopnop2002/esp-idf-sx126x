# VCP Example   
This is LoRa and VCP(Virtual Com Port) gateway application.   

ESP-IDF supports VCP hosts.   
Representative VCP devices include Arduino Uno and Arduino Mega.   
ESP-IDF can communicate with VCP devices using the USB port.   
I based it on [this](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/usb/host/cdc/cdc_acm_vcp).   

This project uses the following components.   
Other UART-USB converter chips are not supported.   
- https://components.espressif.com/components/espressif/usb_host_ch34x_vcp   
- https://components.espressif.com/components/espressif/usb_host_cp210x_vcp   
- https://components.espressif.com/components/espressif/usb_host_ftdi_vcp   


```
           +------------+          +------------+          +------------+
           | VCP device |          |            |          |            |           |
           |Arduino Uno |--(USB)-->|    ESP32   |--(SPI)-->|   SX126x   |==(LoRa)==>
           |Arduino Mega|          |            |          |            |
           +------------+          +------------+          +------------+

           +------------+          +------------+          +------------+
           |            |          |            |          | VCP device |
==(LoRa)==>|   SX126x   |--(SPI)-->|    ESP32   |--(USB)-->|Arduino Uno |
           |            |          |            |          |Arduino MEGA|
           +------------+          +------------+          +------------+
```


# Hardtware requirements

## ESP32-S2/S3
This project only works with ESP32S2/S3.   
The ESP32S2/S3 has USB capabilities.   

## USB Type-A Femail connector
USB connectors are available from AliExpress or eBay.   
I used it by incorporating it into a Universal PCB.   
![USBConnector](https://github.com/user-attachments/assets/8d7d8f0a-d289-44b8-ae90-c693a1099ca0)

We can buy this breakout on Ebay or AliExpress.   
![usb-conector-11](https://github.com/user-attachments/assets/848998d4-fb0c-4b4f-97ae-0b3ae8b8996a)
![usb-conector-12](https://github.com/user-attachments/assets/6fc34dcf-0b13-4233-8c71-07234e8c6d06)


# Configuration
![Image](https://github.com/user-attachments/assets/703b368c-d80c-47e7-a5bf-87021857238a)
![Image](https://github.com/user-attachments/assets/00a49fb9-03f3-48c9-a77e-8283c0699986)


## Radio Setting

### VCP to LoRa
Receive from VCP device and send to LoRa.   
ESP32 acts as the VCP host.   
```
           +------------+          +------------+          +------------+
           | VCP device |          |            |          |            |           |
           |Arduino Uno |--(USB)-->|    ESP32   |--(SPI)-->|   SX126x   |==(LoRa)==>
           |Arduino Mega|          |            |          |            |
           +------------+          +------------+          +------------+
```

![Image](https://github.com/user-attachments/assets/75a8d620-fdf5-439f-a99b-e77e8d549324)


### LoRa to VCP
Receive from LoRa and send to VCP device.   
ESP32 acts as the VCP host.   
```
           +------------+          +------------+          +------------+
           |            |          |            |          | VCP device |
==(LoRa)==>|   SX126x   |--(SPI)-->|    ESP32   |--(USB)-->|Arduino Uno |
           |            |          |            |          |Arduino MEGA|
           +------------+          +------------+          +------------+
```

![Image](https://github.com/user-attachments/assets/f59f37b5-c77d-4f2a-b910-02a489f0a42b)


## VCP Setting

![Image](https://github.com/user-attachments/assets/1adb68c5-e115-4180-a2b7-cdf8e32f4301)


# Write this sketch on Arduino Uno.   
You can use any AtMega microcontroller that has a USB port.   

```
const int LED_PIN = 13;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode( LED_PIN, OUTPUT );
}

void loop() {
  while (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    digitalWrite( LED_PIN, !digitalRead(LED_PIN) );
  }

  if(lastMillis + 1000 <= millis()){
    Serial.print("Hello World ");
    Serial.println(millis());
    lastMillis += 1000;
  }

  delay(1);
}
```

Strings from Arduino to ESP32 are terminated with CR(0x0d)+LF(0x0a).   
This project will remove the termination character and send to LoRa.   
```
I (6020) VCP: Receiving data through CdcAcmDevice
I (6020) VCP: 0x3fca1c04   48 65 6c 6c 6f 20 57 6f  72 6c 64 20 31 35 30 30  |Hello World 1500|
I (6020) VCP: 0x3fca1c14   30 0d 0a                                          |0..|
I (6030) TX: xMessageBufferReceive received=19
I (6040) TX: 0x3fc9e230   48 65 6c 6c 6f 20 57 6f  72 6c 64 20 31 35 30 30  |Hello World 1500|
I (6040) TX: 0x3fc9e240   30
```

The Arduino sketch inputs data with LF as the terminator.   
So strings from the ESP32 to the Arduino must be terminated with LF (0x0a).   
If the string output from the ESP32 to the Arduino is not terminated with LF (0x0a), the Arduino sketch will complete the input with a timeout.   
The default input timeout for Arduino sketches is 1000 milliseconds.   
This project will add a LF to the end and send to Arduino.   
The Arduino sketch will blink the on-board LED.   
```
I (3280) RX: 18 byte packet received:[Hello World 133865]
I (3280) RX: 0x3fc9e120   48 65 6c 6c 6f 20 57 6f  72 6c 64 20 31 33 33 38  |Hello World 1338|
I (3280) RX: 0x3fc9e130   36 35                                             |65|
I (3290) VCP: Sending data through CdcAcmDevice
I (3300) VCP: 0x3fc9f260   48 65 6c 6c 6f 20 57 6f  72 6c 64 20 31 33 33 38  |Hello World 1338|
I (3300) VCP: 0x3fc9f270   36 35 0a
```

# Wireing   
Arduino Uno connects via USB connector.   
The USB port on the ESP32S2/S3 development board does not function as a USB-HOST.   

```
+---------+  +-------------+  +-----------+
|ESP BOARD|==|USB CONNECTOR|==|Arduino Uno|
+---------+  +-------------+  +-----------+
```

```
ESP BOARD          USB CONNECTOR (type A)
                         +--+
5V        -------------> | || VCC
[GPIO19]  -------------> | || D-
[GPIO20]  -------------> | || D+
GND       -------------> | || GND
                         +--+
```

![Image](https://github.com/user-attachments/assets/7bf405af-b1ec-4c7c-87d1-8bbe176e807b)

