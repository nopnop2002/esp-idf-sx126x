/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ra01s.h"

static const char *TAG = "MAIN";

#if CONFIG_PRIMARY

#define TIMEOUT 100

void task_primary(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txData[256]; // Maximum Payload size of SX1261/62/68 is 255
	uint8_t rxData[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		TickType_t nowTick = xTaskGetTickCount();
		int txLen = sprintf((char *)txData, "Hello World %"PRIu32, nowTick);
		//uint8_t len = strlen((char *)txData);

		// Wait for transmission to complete
		if (LoRaSend(txData, txLen, SX126x_TXMODE_SYNC)) {
			//ESP_LOGI(pcTaskGetName(NULL), "Send success");

			bool waiting = true;
			TickType_t startTick = xTaskGetTickCount();
			while(waiting) {
				uint8_t rxLen = LoRaReceive(rxData, sizeof(rxData));
				TickType_t currentTick = xTaskGetTickCount();
				TickType_t diffTick = currentTick - startTick;
				if ( rxLen > 0 ) {
					ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, rxData);
					ESP_LOGI(pcTaskGetName(NULL), "Response time is %"PRIu32" millisecond", diffTick * portTICK_PERIOD_MS);
					waiting = false;
				}
				
				ESP_LOGD(pcTaskGetName(NULL), "diffTick=%"PRIu32, diffTick);
				if (diffTick > TIMEOUT) {
					ESP_LOGW(pcTaskGetName(NULL), "No response within %d ticks", TIMEOUT);
					waiting = false;
				}
				vTaskDelay(1); // Avoid WatchDog alerts
			} // end waiting

		} else {
			ESP_LOGE(pcTaskGetName(NULL), "Send fail");
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_PRIMARY

#if CONFIG_SECONDARY
void task_secondary(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t txData[256]; // Maximum Payload size of SX1261/62/68 is 255
	uint8_t rxData[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		uint8_t rxLen = LoRaReceive(rxData, sizeof(rxData));
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
			printf("rssi=%d[dBm] snr=%d[dB]\n", rssi, snr);

			for(int i=0;i<rxLen;i++) {
				if (isupper(rxData[i])) {
					txData[i] = tolower(rxData[i]);
				} else {
					txData[i] = toupper(rxData[i]);
				}
			}

			// Wait for transmission to complete
			if (LoRaSend(txData, rxLen, SX126x_TXMODE_SYNC)) {
				ESP_LOGD(pcTaskGetName(NULL), "Send success");
			} else {
				ESP_LOGE(pcTaskGetName(NULL), "LoRaSend fail");
			}

		}
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while

	// never reach here
	vTaskDelete( NULL );
}
#endif // CONFIG_SECONDARY

void app_main()
{
	// Initialize LoRa
	LoRaInit();
	int8_t txPowerInDbm = 22;

	uint32_t frequencyInHz = 0;
#if CONFIG_433MHZ
	frequencyInHz = 433000000;
	ESP_LOGI(TAG, "Frequency is 433MHz");
#elif CONFIG_866MHZ
	frequencyInHz = 866000000;
	ESP_LOGI(TAG, "Frequency is 866MHz");
#elif CONFIG_915MHZ
	frequencyInHz = 915000000;
	ESP_LOGI(TAG, "Frequency is 915MHz");
#elif CONFIG_OTHER
	ESP_LOGI(TAG, "Frequency is %dMHz", CONFIG_OTHER_FREQUENCY);
	frequencyInHz = CONFIG_OTHER_FREQUENCY * 1000000;
#endif

#if CONFIG_USE_TCXO
	ESP_LOGW(TAG, "Enable TCXO");
	float tcxoVoltage = 3.3; // use TCXO
	bool useRegulatorLDO = true; // use DCDC + LDO
#else
	ESP_LOGW(TAG, "Disable TCXO");
	float tcxoVoltage = 0.0;  // don't use TCXO
	bool useRegulatorLDO = false;  // use only LDO in all modes
#endif

	//LoRaDebugPrint(true);
	if (LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO) != 0) {
		ESP_LOGE(TAG, "Does not recognize the module");
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
#if CONFIG_ADVANCED
	spreadingFactor = CONFIG_SF_RATE;
	bandwidth = CONFIG_BANDWIDTH;
	codingRate = CONFIG_CODING_RATE;
#endif
	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

#if CONFIG_PRIMARY
	xTaskCreate(&task_primary, "PRIMARY", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_SECONDARY
	xTaskCreate(&task_secondary, "SECONDARY", 1024*4, NULL, 5, NULL);
#endif
}

