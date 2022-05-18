# ping-pong Example   
Send data from primary to secondary.   
In the secondary, the characters are converted and sent back.   


# Configuration   
![config-app](https://user-images.githubusercontent.com/6020549/162128875-d5acb0fb-808a-4b1b-949f-81a163213636.jpg)
![config-ping-pong-1](https://user-images.githubusercontent.com/6020549/162128887-45966abf-5865-4422-b715-cf4a36a9e606.jpg)

When using a transceiver other than 169MHz / 433MHz / 470MHz / 866MHz / 915MHz:   
![config-ping-pong-2](https://user-images.githubusercontent.com/6020549/162128891-ab65d80a-fd83-494c-9eb0-6571ca863ce0.jpg)

When using advanced settings:   
![config-ping-pong-3](https://user-images.githubusercontent.com/6020549/162128895-d1c10cd4-ca60-4b01-aacb-5db840627191.jpg)


LoRa mode has the following three communication parameters.   
1.Signal Bandwidth (= BW)   
2.Error Cording Rate (= CR)   
3.Spreading Factor (= SF)   
The communication speed is faster when BW is large, CR is small, and SF is small.   
However, as the communication speed increases, the reception sensitivity deteriorates, so select the one that best suits your needs.   

- Error coding rate   
1:4/5(Default)   
2:4/6   
3:4/7   
4:4/8   

- Signal Bandwidth   
1:15.63 kHz   
2:31.25 kHz   
3:62.50 kHz   
4:125 kHz(Default)   
5:250 kHz   
6:500 kHz   

- SF rate   
5:SF5   
6:SF6   
7:SF7(Default)   
8:SF8   
9:SF9   
10:SF10   
11:SF11   
12:SF12   

