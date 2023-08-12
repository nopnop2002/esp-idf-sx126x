/*	WebSocket Client Example

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.

	https://qiita.com/zakkied/items/b01945c7b0f78733ad26
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_websocket_client.h"

#if CONFIG_RECEIVER

#define TIMEOUT_SEC 5
#define TIMEOUT_VAL 0x255

static const char *TAG = "CLIENT";

extern MessageBufferHandle_t xMessageBufferTrans;
extern size_t xItemSize;

static TimerHandle_t timeout_signal_timer;

typedef struct {
	TaskHandle_t taskHandle;
	int data_len;
	uint8_t op_code;
	int payload_len;
	int payload_offset;
	char data[256];
} SOCKET_t;

static void timer_cb(TimerHandle_t xTimer)
{
	TaskHandle_t taskHandle = ( TaskHandle_t ) pvTimerGetTimerID( xTimer );
	ESP_LOGW(pcTaskGetName(NULL), "taskHandle=0x%x", (unsigned int)taskHandle);
	//ESP_LOGI(pcTaskGetName(NULL), "timer_cb taskHandle=%d", (int)taskHandle);
	ESP_LOGW(pcTaskGetName(NULL), "No data received for %d seconds", TIMEOUT_SEC);
	xTaskNotify( taskHandle, TIMEOUT_VAL, eSetValueWithOverwrite );
}

#if 0
typedef enum ws_transport_opcodes {
	WS_TRANSPORT_OPCODES_CONT =  0x00,
	WS_TRANSPORT_OPCODES_TEXT =  0x01,
	WS_TRANSPORT_OPCODES_BINARY = 0x02,
	WS_TRANSPORT_OPCODES_CLOSE = 0x08,
	WS_TRANSPORT_OPCODES_PING = 0x09,
	WS_TRANSPORT_OPCODES_PONG = 0x0a,
	WS_TRANSPORT_OPCODES_FIN = 0x80,
	WS_TRANSPORT_OPCODES_NONE = 0x100,	 /*!< not a valid opcode to indicate no message previously received
										  * from the API esp_transport_ws_get_read_opcode() */
} ws_transport_opcodes_t;
#endif

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
	SOCKET_t *socketBuf = data->user_context;
	ESP_LOGI(TAG, "taskHandle=0x%x", (unsigned int)socketBuf->taskHandle);
	switch (event_id) {
	case WEBSOCKET_EVENT_CONNECTED:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
		break;
	case WEBSOCKET_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
		break;
	case WEBSOCKET_EVENT_DATA:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
		ESP_LOGI(TAG, "Received opcode=0x%x", data->op_code);
		if (data->op_code == WS_TRANSPORT_OPCODES_TEXT) {
			ESP_LOGI(TAG, "Received text");
		} else if (data->op_code == WS_TRANSPORT_OPCODES_CLOSE) {
			ESP_LOGW(TAG, "Received closed message with text");
		} else if (data->op_code == WS_TRANSPORT_OPCODES_PING) {
			ESP_LOGW(TAG, "Received ping message");
		} else if (data->op_code == WS_TRANSPORT_OPCODES_PONG) {
			ESP_LOGW(TAG, "Received pong message");
		} else if (data->op_code == WS_TRANSPORT_OPCODES_FIN) {
			ESP_LOGW(TAG, "Received fin message");
		} else {
			ESP_LOGE(TAG, "Received unknown message");
		}

		if (data->op_code == WS_TRANSPORT_OPCODES_TEXT || data->op_code == WS_TRANSPORT_OPCODES_CLOSE) {
			ESP_LOGD(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d", data->payload_len, data->data_len, data->payload_offset);
			socketBuf->data_len = data->data_len;
			socketBuf->op_code = data->op_code;
			socketBuf->payload_len = data->payload_len;
			socketBuf->payload_offset = data->payload_offset;
			for(int i=0;i<data->data_len;i++) {
				socketBuf->data[i] = data->data_ptr[i];
				socketBuf->data[i+1] = 0;
			}
			xTaskNotify( socketBuf->taskHandle, event_id, eSetValueWithOverwrite );
		}
		break;
	case WEBSOCKET_EVENT_ERROR:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
		break;
	}
}

esp_err_t query_mdns_host(const char * host_name, char *ip);
void convert_mdns_host(char * from, char * to);

void ws_client(void *pvParameters)
{
	ESP_LOGI(TAG, "Start WEB_SERVER_HOST:%s WEB_SERVER_PORT:%d", CONFIG_WEB_SERVER_HOST, CONFIG_WEB_SERVER_PORT);

	// Resolve mDNS host name
	char ip[128];
	ESP_LOGI(TAG, "CONFIG_WEB_SERVER_HOST=[%s]", CONFIG_WEB_SERVER_HOST);
	convert_mdns_host(CONFIG_WEB_SERVER_HOST, ip);
	ESP_LOGI(TAG, "ip=[%s]", ip);
	char url[142];
	sprintf(url, "ws://%s:%d", ip, CONFIG_WEB_SERVER_PORT);
	ESP_LOGI(TAG, "url=[%s]", url);

	TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
	ESP_LOGI(TAG, "taskHandle=0x%x", (unsigned int)taskHandle);
	timeout_signal_timer = xTimerCreate("timer", TIMEOUT_SEC * 1000 / portTICK_PERIOD_MS,
		pdFALSE, taskHandle, timer_cb);

	SOCKET_t socketBuf;
	socketBuf.taskHandle = taskHandle;
	esp_websocket_client_config_t websocket_cfg = {};
	websocket_cfg.uri = url;
	websocket_cfg.user_context=&socketBuf;
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
	websocket_cfg.reconnect_timeout_ms = 10000;
	websocket_cfg.network_timeout_ms = 10000;
	websocket_cfg.ping_interval_sec = 60;
#endif

	ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);
	esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
	esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

	// Wait for server connection
	esp_websocket_client_start(client);
	while(1) {
		if (esp_websocket_client_is_connected(client)) break;
		vTaskDelay(1);
	}
	ESP_LOGI(TAG, "Connected to %s...", websocket_cfg.uri);

	char buffer[xItemSize];
	while (1) {
		size_t received = xMessageBufferReceive(xMessageBufferTrans, buffer, sizeof(buffer), portMAX_DELAY);
		ESP_LOGI(TAG, "xMessageBufferReceive received=%d", received);
		if (received > 0) {
			ESP_LOGI(TAG, "xMessageBufferReceive buffer=[%.*s]",received, buffer);
			if (esp_websocket_client_is_connected(client)) {
				xTimerStart(timeout_signal_timer, portMAX_DELAY);
				esp_websocket_client_send_text(client, buffer, received, portMAX_DELAY);
				uint32_t event_id = ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
				ESP_LOGI(TAG, "ulTaskNotifyTake=0x%"PRIx32, event_id);
				if (event_id != TIMEOUT_VAL){
					ESP_LOGI(TAG, "Receiving op_code=%d data_len=%d", socketBuf.op_code, socketBuf.data_len);
					ESP_LOGI(TAG, "DATA=[%.*s]\r", socketBuf.data_len, socketBuf.data);
					if (socketBuf.op_code == WS_TRANSPORT_OPCODES_CLOSE) break;
				} else {
					ESP_LOGW(TAG, "Receive timeout");
				}
				xTimerStop(timeout_signal_timer, 100);
			} else {
				ESP_LOGW(TAG, "Not connected server");
			}
		} else {
			 ESP_LOGW(TAG, "xMessageBufferReceive fail");
			 //break;
		} // end while
	}

	// Stop connection
	// stops ws client and closes TCP connection directly with sending close frames.
	//esp_websocket_client_close(client, 1000 / portTICK_PERIOD_MS);
	// stops ws client and closes TCP connection directly without sending close frames.
	esp_websocket_client_stop(client);
	ESP_LOGI(TAG, "Websocket client destory");
	esp_websocket_client_destroy(client);
	vTaskDelete(NULL);
}
#endif
