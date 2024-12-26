# tinyusb Example   
This is LoRa and tinyusb gateway application.   
```
           +------------+          +------------+          +------------+
           |            |          |            |          |            |
           | USB Writer |--(USB)-->|    ESP32   |--(SPI)-->|   SX126x   |==(LoRa)==>
           |            |          |            |          |            |
           +------------+          +------------+          +------------+

           +------------+          +------------+          +------------+
           |            |          |            |          |            |
==(LoRa)==>|   SX126x   |--(SPI)-->|    ESP32   |--(USB)-->| USB Reader |
           |            |          |            |          |            |
           +------------+          +------------+          +------------+
```

# Hardware requirements
1. ESP32-S2/S3 Development board   
Because the ESP32-S2/S3 does support USB OTG.   

2. USB Connector   
I used this:   
![usb-connector](https://user-images.githubusercontent.com/6020549/124848149-3714ba00-dfd7-11eb-8344-8b120790c5c5.JPG)

```
ESP32-S2/S3 BOARD          USB CONNECTOR
                           +--+
                           | || VCC
    [GPIO 19]    --------> | || D-
    [GPIO 20]    --------> | || D+
    [  GND  ]    --------> | || GND
                           +--+
```


# Configuration
![config-top](https://github.com/user-attachments/assets/64725b68-2185-4848-9e90-34f47decb81d)

### USB to LoRa
Subscribe with USB and send to LoRa.   
ESP32 acts as USB Reader.   
You can use this script as USB Writer.   
```python3 ./write.py```

```
           +------------+          +------------+          +------------+
           |            |          |            |          |            |
           | USB Writer |--(USB)-->|    ESP32   |--(SPI)-->|   SX126x   |==(LoRa)==>
           |            |          |            |          |            |
           +------------+          +------------+          +------------+
```

![config-radio-1](https://github.com/user-attachments/assets/6b75c21a-5cec-4af8-8b75-f6a541d4900a)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_RX   


### LoRa to USB
Receive from LoRa and publish as USB.   
ESP32 acts as USB Writer.   
You can use this script as USB Reader.   
```python3 ./read.py```

```
           +------------+          +------------+          +------------+
           |            |          |            |          |            |
==(LoRa)==>|   SX126x   |--(SPI)-->|    ESP32   |--(USB)-->| USB Reader |
           |            |          |            |          |            |
           +------------+          +------------+          +------------+
```

![config-radio-2](https://github.com/user-attachments/assets/0d685209-a8bd-4925-a1e3-4c4add64ca83)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\Ra01S_TX   


# Using Windows Terminal Software
When you connect the USB cable to the USB port on your Windows machine and build the firmware, a new COM port will appear.   
Open a new COM port in the terminal software.   
When set to USB to LoRa, input data from the keyboard is sent to LoRa.   
I used TeraTerm.   
![tusb-TeraTerm](https://github.com/user-attachments/assets/b5eea94e-5228-45b4-bcad-81cce8c52479)

# Using Linux Terminal Software
When you connect the USB cable to the USB port on your Linux machine and build the firmware, a new /dev/tty device will appear.   
Open a new tty device in the terminal software.   
Most occasions, the device is /dev/ttyACM0.   
When set to USB to LoRa, input data from the keyboard is sent to LoRa.   
I used screen.   
![tusb-screen](https://github.com/user-attachments/assets/18a6e519-9250-4109-b05d-6bcd418bfb5b)

