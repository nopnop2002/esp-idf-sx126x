/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
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
		int txLen = sprintf((char *)txData, "Hello World %"PRIu32, nowTick);
		ESP_LOGI(pcTaskGetName(NULL), "%d byte packet sent...", txLen);

		// Wait for transmission to complete
		if (LoRaSend(txData, txLen, SX126x_TXMODE_SYNC) == false) {
			ESP_LOGE(pcTaskGetName(NULL),"LoRaSend fail");
		}

		// Do not wait for the transmission to be completed
		//LoRaSend(txData, txLen, SX126x_TXMODE_ASYNC );
		int lost = GetPacketLost();
		if (lost != 0) {
			ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
		}

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
			ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, rxData);

			int8_t rssi, snr;
			GetPacketStatus(&rssi, &snr);
			ESP_LOGI(pcTaskGetName(NULL), "rssi=%d[dBm] snr=%d[dB]", rssi, snr);
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
	ESP_LOGI(TAG, "Frequency is 169MHz");
#elif CONFIG_433MHZ
	frequencyInHz = 433000000;
	ESP_LOGI(TAG, "Frequency is 433MHz");
#elif CONFIG_470MHZ
	frequencyInHz = 470000000;
	ESP_LOGI(TAG, "Frequency is 470MHz");
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

	LoRaInit();
	int8_t txPowerInDbm = 22;
#if 1
	float tcxoVoltage = 0.0; // don't use TCXO
	bool useRegulatorLDO = false; // use only LDO in all modes
#else
	float tcxoVoltage = 3.3; // use TCXO
	bool useRegulatorLDO = true; // use DCDC + LDO
#endif
	//LoRaDebugPrint(true);
	int ret = LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO);
	ESP_LOGI(TAG, "LoRaBegin=%d", ret);
	if (ret != 0) {
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
#if CONFIF_ADVANCED
	spreadingFactor = CONFIG_SF_RATE;
	bandwidth = CONFIG_BANDWIDTH;
	codingRate = CONFIG_CODING_RATE
#endif
	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

#if CONFIG_SENDER
	xTaskCreate(&task_tx, "TX", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&task_rx, "RX", 1024*4, NULL, 1, NULL);
#endif
}

