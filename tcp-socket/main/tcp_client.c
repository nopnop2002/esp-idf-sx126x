/*	BSD Socket TCP Client

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
#include "mdns.h"

#include "lwip/sockets.h"

static const char *TAG = "TCP-CLIENT";

extern MessageBufferHandle_t xMessageBufferTrans;

esp_err_t query_mdns_host(const char * host_name, char *ip)
{
	ESP_LOGD(__FUNCTION__, "Query A: %s", host_name);

	struct esp_ip4_addr addr;
	addr.addr = 0;

	esp_err_t err = mdns_query_a(host_name, 10000,	&addr);
	if(err){
		if(err == ESP_ERR_NOT_FOUND){
			ESP_LOGW(__FUNCTION__, "%s: Host was not found!", esp_err_to_name(err));
			return ESP_FAIL;
		}
		ESP_LOGE(__FUNCTION__, "Query Failed: %s", esp_err_to_name(err));
		return ESP_FAIL;
	}

	ESP_LOGD(__FUNCTION__, "Query A: %s.local resolved to: " IPSTR, host_name, IP2STR(&addr));
	sprintf(ip, IPSTR, IP2STR(&addr));
	return ESP_OK;
}


void convert_mdns_host(char * from, char * to)
{
	ESP_LOGI(__FUNCTION__, "from=[%s]",from);
	strcpy(to, from);
	char *sp;
	sp = strstr(from, ".local");
	if (sp == NULL) return;

	int _len = sp - from;
	ESP_LOGD(__FUNCTION__, "_len=%d", _len);
	char _from[128];
	strcpy(_from, from);
	_from[_len] = 0;
	ESP_LOGI(__FUNCTION__, "_from=[%s]", _from);

	char _ip[128];
	esp_err_t ret = query_mdns_host(_from, _ip);
	ESP_LOGI(__FUNCTION__, "query_mdns_host=%d _ip=[%s]", ret, _ip);
	if (ret != ESP_OK) return;

	strcpy(to, _ip);
	ESP_LOGI(__FUNCTION__, "to=[%s]", to);
}

int connectServer(struct sockaddr_in dest_addr) {
	int addr_family = AF_INET;
	int ip_protocol = IPPROTO_IP;
	int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (sock < 0) {
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		return -1;
	}
	ESP_LOGI(TAG, "Socket created, connecting to %s:%d", CONFIG_TCP_HOST, CONFIG_TCP_PORT);

	int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
	if (err != 0) {
		ESP_LOGW(TAG, "Socket unable to connect: errno %d", errno);
		close(sock);
		return -1;
	}
	return sock;
}

void tcp_client(void *pvParameters)
{
	ESP_LOGI(TAG, "Start HOST=[%s] PORT=%d", CONFIG_TCP_HOST, CONFIG_TCP_PORT);

	// Initialize mDNS
	ESP_ERROR_CHECK( mdns_init() );

	// Resolve mDNS host name
	char ip[128];
	convert_mdns_host(CONFIG_TCP_HOST, ip);
	ESP_LOGI(TAG, "ip=[%s]", ip);

	struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(CONFIG_TCP_PORT);
	//dest_addr.sin_addr.s_addr = inet_addr(CONFIG_TCP_HOST);
	dest_addr.sin_addr.s_addr = inet_addr(ip);

	while (1) {
		char buffer[256]; // Maximum Payload size of SX1261/62/68 is 255
		size_t received = xMessageBufferReceive(xMessageBufferTrans, buffer, sizeof(buffer), portMAX_DELAY);
		ESP_LOGI(TAG, "xMessageBufferReceive received=%d", received);
		if (received > 0) {
			int sock = connectServer(dest_addr);
			if (sock < 0) continue;
			ESP_LOGI(TAG, "xMessageBufferReceive buffer=[%.*s]",received, buffer);
			int sended = send(sock, buffer, received, 0);
			ESP_LOGI(TAG, "send sended=%d", sended);
			if (sended != received) {
				ESP_LOGE(TAG, "send fail received=%d sended=%d", received, sended);
				break;
			}

			shutdown(sock, 0);
			close(sock);

		} else {
			ESP_LOGE(TAG, "xMessageBufferReceive fail");
			break;
		}
	} // end while

	vTaskDelete(NULL);
}
