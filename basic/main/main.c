/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ra01s.h"

#define TAG "MAIN"

#if CONFIG_SENDER
void task_tx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txData[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		TickType_t nowTick = xTaskGetTickCount();
		int txLen = sprintf((char *)txData, "Hello World %d", nowTick);

		// Wait for transmission to complete
		if (LoRaSend(txData, txLen, SX126x_TXMODE_SYNC)) {
			//printf("Send success\n");
		} else {
			printf("Send fail\n");
		}

		// Do not wait for the transmission to be completed
		//LoRaSend(txData, txLen, SX126x_TXMODE_ASYNC );

		vTaskDelay(pdMS_TO_TICKS(1000));
	} // end while
}
#endif // CONFIG_SENDER



#if CONFIG_RECEIVER
void task_rx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t rxData[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		uint8_t rxLen = LoRaReceive(rxData, 255);
		if ( rxLen > 0 ) { 
			printf("Receive rxLen:%d\n", rxLen);
			for(int i=0;i< rxLen;i++) {
				printf("%02x ",rxData[i]);
			}
			printf("\n");

			for(int i=0;i< rxLen;i++) {
				if (rxData[i] > 0x19 && rxData[i] < 0x7F) {
					char myChar = rxData[i];
					printf("%c", myChar);
				} else {
					printf("?");
				}
			}
			printf("\n");

			int8_t rssi, snr;
			GetPacketStatus(&rssi, &snr);
			printf("rssi:%d dBm snr=%d dB\n", rssi, snr);
		}
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while
}
#endif // CONFIG_RECEIVER

void app_main()
{
	uint32_t frequencyInHz = 0;
#if CONFIG_169MHZ
	frequencyInHz = 169000000;
	ESP_LOGI(pcTaskGetName(NULL), "Frequency is 169MHz");
#elif CONFIG_433MHZ
	frequencyInHz = 433000000;
	ESP_LOGI(pcTaskGetName(NULL), "Frequency is 433MHz");
#elif CONFIG_470MHZ
	frequencyInHz = 470000000;
	ESP_LOGI(pcTaskGetName(NULL), "Frequency is 470MHz");
#elif CONFIG_866MHZ
	frequencyInHz = 866000000;
	ESP_LOGI(pcTaskGetName(NULL), "Frequency is 866MHz");
#elif CONFIG_915MHZ
	frequencyInHz = 915000000;
	ESP_LOGI(pcTaskGetName(NULL), "Frequency is 915MHz");
#elif CONFIG_OTHER
	ESP_LOGI(pcTaskGetName(NULL), "Frequency is %dMHz", CONFIG_OTHER_FREQUENCY);
	frequencyInHz = CONFIG_OTHER_FREQUENCY * 1000000;
#endif

	LoRaInit();
	//int ret = LoRaBegin(915000000, 22, 0.0, false);
	int8_t txPowerInDbm = 22;
	float tcxoVoltage = 0.0; // don't use TCXO
	bool useRegulatorLDO = false; // use only LDO in all modes
	int ret = LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO);
	ESP_LOGI(TAG, "LoRaBegin=%d", ret);
	if (ret != 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Does not recognize the module");
		while(1) {
			vTaskDelay(1);
		}
	}
	
	uint8_t spreadingFactor = 7;
	uint8_t bandwidth = 4;
	uint8_t codingRate = 1;
	uint16_t preambleLength = 8;
	uint8_t payloadLen = 0;
	bool crcOn = true;
	bool invertIrq = false;
#if CONFIF_ADVANCED
	spreadingFactor = CONFIG_SF_RATE;
	bandwidth = CONFIG_BANDWIDTH;
	codingRate = CONFIG_CODING_RATE
#endif
	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

	//LoRaDebugPrint(true);

#if CONFIG_SENDER
	xTaskCreate(&task_tx, "task_tx", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&task_rx, "task_rx", 1024*4, NULL, 1, NULL);
#endif
}

