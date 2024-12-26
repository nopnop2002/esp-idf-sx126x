/* WebSocket Server Example

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
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "SERVER";

extern MessageBufferHandle_t xMessageBufferRecv;

static esp_err_t root_get_handler(httpd_req_t *req)
{
	if (req->method == HTTP_GET) {
		ESP_LOGI(TAG, "Handshake done, the new connection was opened");
		return ESP_OK;
	}
	httpd_ws_frame_t ws_pkt;
	uint8_t *buf = NULL;
	memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
	ws_pkt.type = HTTPD_WS_TYPE_TEXT;
	/* Set max_len = 0 to get the frame len */
	esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
		return ret;
	}
	ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
	if (ws_pkt.len) {
		/* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
		buf = calloc(1, ws_pkt.len + 1);
		if (buf == NULL) {
			ESP_LOGE(TAG, "Failed to calloc memory for buf");
			return ESP_ERR_NO_MEM;
		}
		ws_pkt.payload = buf;
		/* Set max_len = ws_pkt.len to get the frame payload */
		ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
			free(buf);
			return ret;
		}
		ESP_LOGI(TAG, "Got packet with message: [%.*s]", ws_pkt.len, ws_pkt.payload);

		// Queries a message buffer to see how much free space it contains
		size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferRecv );
		ESP_LOGI(TAG, "spacesAvailable=%d", spacesAvailable);
		size_t sended = xMessageBufferSend(xMessageBufferRecv, ws_pkt.payload, ws_pkt.len, 100);
		if (sended != ws_pkt.len) {
			ESP_LOGE(TAG, "xMessageBufferSend fail. ws_pkt.len=%d sended=%d", ws_pkt.len, sended);
		}

		ESP_LOGD(TAG, "Packet final: %d", ws_pkt.final);
		ESP_LOGD(TAG, "Packet fragmented: %d", ws_pkt.fragmented);
		ESP_LOGD(TAG, "Packet type: %d", ws_pkt.type);

		strcpy((char *)ws_pkt.payload, "ok");
		ws_pkt.len = 2;
		ret = httpd_ws_send_frame(req, &ws_pkt);
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
		}
		free(buf);
	}
	return ret;
}

/* Function to start the web server */
esp_err_t start_server(int port)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.server_port = port;

	// Start the httpd server
	if (httpd_start(&server, &config) != ESP_OK) {
		ESP_LOGE(TAG, "Failed to starting server!");
		return ESP_FAIL;
	}

	// Registering the handler
	ESP_LOGI(TAG, "Registering URI handlers");
	httpd_uri_t _root_get_handler = {
		.uri			= "/",
		.method			= HTTP_GET,
		.handler		= root_get_handler,
		.user_ctx		= NULL,
		.is_websocket	= true
	};
	httpd_register_uri_handler(server, &_root_get_handler);

	return ESP_OK;
}

void ws_server(void *pvParameters)
{
	char *task_parameter = (char *)pvParameters;
	ESP_LOGI(TAG, "Start task_parameter=%s", task_parameter);
	char url[64];
	int port = CONFIG_WEB_SERVER_PORT;
	sprintf(url, "ws://%s:%d", task_parameter, port);
	ESP_LOGI(TAG, "Starting HTTP server on %s", url);
	ESP_ERROR_CHECK(start_server(port));
	vTaskDelete(NULL);
}
