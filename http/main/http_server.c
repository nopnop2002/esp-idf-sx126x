/*	Simple HTTP Server Example

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
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "esp_http_server.h"

#if CONFIG_SENDER

static const char *TAG = "SERVER";

extern MessageBufferHandle_t xMessageBufferRecv;
extern size_t xItemSize;

/* root post handler */
static esp_err_t root_post_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "root_post_handler. req->content_len=%d", req->content_len);

	/* Allocate memory */
	char *buf = NULL;
	buf = malloc(req->content_len);
	if (buf == NULL) {
		ESP_LOGE(TAG, "malloc fail. req->content_len=%d", req->content_len);
		return ESP_FAIL;
	}

	/* Read the data for the request */
	if (httpd_req_recv(req, buf, req->content_len) != req->content_len) {
		ESP_LOGE(TAG, "httpd_req_recv fail");
		free(buf);
		return ESP_FAIL;
	}

	/* Log data received */
	ESP_LOGI(TAG, "%.*s", req->content_len, buf);
	size_t sended = xMessageBufferSend(xMessageBufferRecv, buf, req->content_len, portMAX_DELAY);
	if (sended != req->content_len) {
		ESP_LOGE(TAG, "xMessageBufferSend fail. sended=%d req->content_len=%d", sended, req->content_len);
	}
	free(buf);

	/* Send response */
	httpd_resp_sendstr_chunk(req, "OK");

	/* Send empty chunk to signal HTTP response completion */
	httpd_resp_sendstr_chunk(req, NULL);
	return ESP_OK;
}

/* favicon get handler */
static esp_err_t favicon_get_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "favicon_get_handler");
	return ESP_OK;
}

/* Function to start the web server */
esp_err_t start_server(int port)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	// Purge“"Least Recently Used” connection
	config.lru_purge_enable = true;
	// TCP Port number for receiving and transmitting HTTP traffic
	config.server_port = port;

	// Start the httpd server
	if (httpd_start(&server, &config) != ESP_OK) {
		ESP_LOGE(TAG, "Failed to starting server!");
		return ESP_FAIL;
	}

	// Set URI handlers
	httpd_uri_t _root_post_handler = {
		.uri		= "/post",
		.method		= HTTP_POST,
		.handler	= root_post_handler,
		.user_ctx	= NULL,
	};
	httpd_register_uri_handler(server, &_root_post_handler);

	httpd_uri_t _favicon_get_handler = {
		.uri		= "/favicon.ico",
		.method		= HTTP_GET,
		.handler	= favicon_get_handler,
		.user_ctx	= NULL,
	};
	httpd_register_uri_handler(server, &_favicon_get_handler);

	return ESP_OK;
}

void http_server(void *pvParameters)
{
	char *task_parameter = (char *)pvParameters;
	ESP_LOGI(TAG, "Start task_parameter=%s", task_parameter);
	char url[64];
	int port = CONFIG_WEB_SERVER_PORT;
	sprintf(url, "http://%s:%d", task_parameter, port);
	ESP_LOGI(TAG, "Starting HTTP server on %s", url);
	ESP_ERROR_CHECK(start_server(port));
	vTaskDelete(NULL);
}
#endif
