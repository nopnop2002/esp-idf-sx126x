/*	MQTT (over TCP) Example

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "esp_event.h"
#include "lwip/dns.h"
#include "esp_mac.h"
#include "mqtt_client.h"

#include "mqtt.h"

static const char *TAG = "SUB";

extern const uint8_t root_cert_pem_start[] asm("_binary_root_cert_pem_start");
extern const uint8_t root_cert_pem_end[] asm("_binary_root_cert_pem_end");

extern MessageBufferHandle_t xMessageBufferRecv;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = event_data;
	MQTT_t *mqttBuf = handler_args;
	ESP_LOGI(TAG, "taskHandle=0x%x", (unsigned int)mqttBuf->taskHandle);
	mqttBuf->event_id = event->event_id;
	switch (event->event_id) {
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
			xTaskNotifyGive( mqttBuf->taskHandle );
			break;
		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
			xTaskNotifyGive( mqttBuf->taskHandle );
			break;
		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_UNSUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_DATA:
			ESP_LOGI(TAG, "MQTT_EVENT_DATA");
			ESP_LOGI(TAG, "TOPIC=[%.*s] DATA=[%.*s]\r", event->topic_len, event->topic, event->data_len, event->data);

			mqttBuf->topic_len = event->topic_len;
			for(int i=0;i<event->topic_len;i++) {
				mqttBuf->topic[i] = event->topic[i];
				mqttBuf->topic[i+1] = 0;
			}
			mqttBuf->data_len = event->data_len;
			for(int i=0;i<event->data_len;i++) {
				mqttBuf->data[i] = event->data[i];
				mqttBuf->data[i+1] = 0;
			}
			xTaskNotifyGive( mqttBuf->taskHandle );
			break;
		case MQTT_EVENT_ERROR:
			ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
			xTaskNotifyGive( mqttBuf->taskHandle );
			break;
		default:
			ESP_LOGI(TAG, "Other event id:%d", event->event_id);
			break;
	}
	return;
}

esp_err_t query_mdns_host(const char * host_name, char *ip);
void convert_mdns_host(char * from, char * to);

void mqtt_sub(void *pvParameters)
{
	ESP_LOGI(TAG, "Start");
	ESP_LOGI(TAG, "CONFIG_MQTT_BROKER=[%s]", CONFIG_MQTT_BROKER);

	// Set client id from mac
	uint8_t mac[8];
	ESP_ERROR_CHECK(esp_base_mac_addr_get(mac));
	for(int i=0;i<8;i++) {
		ESP_LOGD(TAG, "mac[%d]=%x", i, mac[i]);
	}
	char client_id[64];
	sprintf(client_id, "esp32-%02x%02x%02x%02x%02x%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	ESP_LOGI(TAG, "client_id=[%s]", client_id);

	// Resolve mDNS host name
	char ip[128];
	char uri[128];
	ESP_LOGI(TAG, "CONFIG_MQTT_BROKER=[%s]", CONFIG_MQTT_BROKER);
	convert_mdns_host(CONFIG_MQTT_BROKER, ip);
	ESP_LOGI(TAG, "ip=[%s]", ip);
#if CONFIG_MQTT_TRANSPORT_OVER_TCP
	ESP_LOGI(TAG, "MQTT_TRANSPORT_OVER_TCP");
	sprintf(uri, "mqtt://%.60s:%d", ip, CONFIG_MQTT_PORT_TCP);
#elif CONFIG_MQTT_TRANSPORT_OVER_SSL
	ESP_LOGI(TAG, "MQTT_TRANSPORT_OVER_SSL");
	sprintf(uri, "mqtts://%.60s:%d", ip, CONFIG_MQTT_PORT_SSL);
#elif CONFIG_MQTT_TRANSPORT_OVER_WS
	ESP_LOGI(TAG, "MQTT_TRANSPORT_OVER_WS");
	sprintf(uri, "ws://%.60s:%d/mqtt", ip, CONFIG_MQTT_PORT_WS);
#elif CONFIG_MQTT_TRANSPORT_OVER_WSS
	ESP_LOGI(TAG, "MQTT_TRANSPORT_OVER_WSS");
	sprintf(uri, "wss://%.60s:%d/mqtt", ip, CONFIG_MQTT_PORT_WSS);
#endif
	ESP_LOGI(TAG, "uri=[%s]", uri);

	// Initialize user context
	MQTT_t mqttBuf;
	mqttBuf.taskHandle = xTaskGetCurrentTaskHandle();
	ESP_LOGI(TAG, "taskHandle=0x%x", (unsigned int)mqttBuf.taskHandle);

	// Initialize MQTT configuration structure
	esp_mqtt_client_config_t mqtt_cfg = {
		.broker.address.uri = uri,
#if CONFIG_MQTT_TRANSPORT_OVER_TCP
#elif CONFIG_MQTT_TRANSPORT_OVER_SSL
		.broker.verification.certificate = (const char *)root_cert_pem_start,
#elif CONFIG_MQTT_TRANSPORT_OVER_WS
#elif CONFIG_MQTT_TRANSPORT_OVER_WSS
		.broker.verification.certificate = (const char *)root_cert_pem_start,
#endif
#if CONFIG_BROKER_AUTHENTICATION
		.credentials.username = CONFIG_AUTHENTICATION_USERNAME,
		.credentials.authentication.password = CONFIG_AUTHENTICATION_PASSWORD,
#endif
		.credentials.client_id = client_id
	};

#if CONFIG_MQTT_PROTOCOL_V_3_1_1
	ESP_LOGI(TAG, "MQTT_PROTOCOL_V_3_1_1");
	mqtt_cfg.session.protocol_ver = MQTT_PROTOCOL_V_3_1_1;
#elif CONFIG_MQTT_PROTOCOL_V_5
	ESP_LOGI(TAG, "MQTT_PROTOCOL_V_5");
	mqtt_cfg.session.protocol_ver = MQTT_PROTOCOL_V_5;
#endif

	esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, &mqttBuf);
	esp_mqtt_client_start(mqtt_client);

	while (1) {
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		ESP_LOGI(TAG, "ulTaskNotifyTake");
		ESP_LOGI(TAG, "event_id=%"PRIi32, mqttBuf.event_id);

		if (mqttBuf.event_id == MQTT_EVENT_CONNECTED) {
			esp_mqtt_client_subscribe(mqtt_client, CONFIG_MQTT_SUB_TOPIC, 0);
			ESP_LOGI(TAG, "Subscribe to MQTT Server");
		} else if (mqttBuf.event_id == MQTT_EVENT_DISCONNECTED) {
			break;
		} else if (mqttBuf.event_id == MQTT_EVENT_DATA) {
			ESP_LOGI(TAG, "TOPIC=[%.*s]\r", mqttBuf.topic_len, mqttBuf.topic);
			ESP_LOGI(TAG, "DATA=[%.*s]\r", mqttBuf.data_len, mqttBuf.data);

			// Queries a message buffer to see how much free space it contains
			size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferRecv );
			ESP_LOGI(pcTaskGetName(NULL), "spacesAvailable=%d", spacesAvailable);
			size_t sended = xMessageBufferSend(xMessageBufferRecv, mqttBuf.data, mqttBuf.data_len, 100);
			if (sended != mqttBuf.data_len) {
				ESP_LOGE(TAG, "xMessageBufferSend fail mqttBuf.data_len=%d sended=%d", mqttBuf.data_len, sended);
				break;
			}
		} else if (mqttBuf.event_id == MQTT_EVENT_ERROR) {
			break;
		}
	} // end while

	esp_mqtt_client_stop(mqtt_client);
	vTaskDelete(NULL);
}
