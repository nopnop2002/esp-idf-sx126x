/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "ra01s.h"

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event */
/* - are we connected to the AP with an IP? */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "MAIN";

static int s_retry_num = 0;

MessageBufferHandle_t xMessageBufferTrans;
MessageBufferHandle_t xMessageBufferRecv;

// The total number of bytes (not single messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 1024;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
		}
		ESP_LOGI(TAG,"connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		s_retry_num = 0;
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void wifi_init_sta(void)
{
	s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
		ESP_EVENT_ANY_ID,
		&event_handler,
		NULL,
		&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
		IP_EVENT_STA_GOT_IP,
		&event_handler,
		NULL,
		&instance_got_ip));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = CONFIG_ESP_WIFI_SSID,
			.password = CONFIG_ESP_WIFI_PASSWORD,
			/* Setting a password implies station will connect to all security modes including WEP/WPA.
			 * However these modes are deprecated and not advisable to be used. Incase your Access point
			 * doesn't support WPA2, these mode can be enabled by commenting below line */
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,

			.pmf_cfg = {
				.capable = true,
				.required = false
			},
		},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
		WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
		pdFALSE,
		pdFALSE,
		portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
	 * happened. */
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
	} else {
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}

	/* The event will not be processed after unregister */
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
	vEventGroupDelete(s_wifi_event_group);
}


#if CONFIG_SENDER
void task_tx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		size_t received = xMessageBufferReceive(xMessageBufferRecv, buf, sizeof(buf), portMAX_DELAY);
		ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);

		// Transmission begins
		ESP_LOGD(pcTaskGetName(NULL), "Transmission begin");

		// Wait for transmission to complete
		if (LoRaSend(buf, received, SX126x_TXMODE_SYNC) == false) {
			ESP_LOGE(pcTaskGetName(NULL),"LoRaSend fail");
		}

		// Do not wait for the transmission to be completed
		//LoRaSend(buf, received, SX126x_TXMODE_ASYNC );

		int lost = GetPacketLost();
		if (lost != 0) {
			ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
		}

		// Transmission is complete
		ESP_LOGD(pcTaskGetName(NULL), "Transmission done");
	} // end while
	vTaskDelete(NULL);
}
#endif // CONFIG_SENDER

#if CONFIG_RECEIVER
void task_rx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[256]; // Maximum Payload size of SX1261/62/68 is 255
	while(1) {
		uint8_t rxLen = LoRaReceive(buf, sizeof(buf));
		if ( rxLen > 0 ) { 
			ESP_LOGI(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rxLen, rxLen, buf);

			int8_t rssi, snr;
			GetPacketStatus(&rssi, &snr);
			ESP_LOGI(pcTaskGetName(NULL), "rssi=%d[dBm] snr=%d[dB]", rssi, snr);

			size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferTrans );
			ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
			size_t sended = xMessageBufferSend(xMessageBufferTrans, buf, rxLen, 100);
			if (sended != rxLen) {
				ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSend fail rxLen=%d sended=%d", rxLen, sended);
				break;
			}
		}
		vTaskDelay(1); // Avoid WatchDog alerts
	} // end while
	vTaskDelete(NULL);
}
#endif // CONFIG_RECEIVER

void udp_server(void *pvParameters);
void udp_client(void *pvParameters);

void app_main()
{
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// Initialize WiFi
	wifi_init_sta();

	// Create MessageBuffer
	xMessageBufferTrans = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferTrans );
	xMessageBufferRecv = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferRecv );

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

	// Initialize LoRa
	LoRaInit();
	int8_t txPowerInDbm = 22;

#if CONFIG_USE_TCXO
	ESP_LOGW(TAG, "Enable TCXO");
	float tcxoVoltage = 3.3; // use TCXO
	bool useRegulatorLDO = true; // use DCDC + LDO
#else
	ESP_LOGW(TAG, "Disable TCXO");
	float tcxoVoltage = 0.0; // don't use TCXO
	bool useRegulatorLDO = false; // use only LDO in all modes
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
#if CONFIF_ADVANCED
	spreadingFactor = CONFIG_SF_RATE;
	bandwidth = CONFIG_BANDWIDTH;
	codingRate = CONFIG_CODING_RATE
#endif
	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

#if CONFIG_SENDER
	xTaskCreate(&task_tx, "TX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&udp_server, "UDP_SERVER", 1024*4, NULL, 5, NULL);
#endif
#if CONFIG_RECEIVER
	xTaskCreate(&task_rx, "RX", 1024*4, NULL, 5, NULL);
	xTaskCreate(&udp_client, "UDP_CLIENT", 1024*4, NULL, 5, NULL);
#endif
}

