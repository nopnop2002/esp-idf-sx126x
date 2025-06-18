/*
	BSD Socket UDP Broadcast Server

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "mdns.h"

static const char *TAG = "UDP-SERVER";

extern MessageBufferHandle_t xMessageBufferRecv;
extern size_t xItemSize;

void udp_server(void *pvParameters)
{
	ESP_LOGI(TAG, "Start UDP PORT=%d", CONFIG_UDP_PORT);

	// Initialize mDNS
	ESP_ERROR_CHECK( mdns_init() );

	// Set mDNS hostname (required if you want to advertise services)
	ESP_ERROR_CHECK( mdns_hostname_set(CONFIG_MDNS_HOSTNAME) );
	ESP_LOGI(TAG, "mdns hostname set to: [%s]", CONFIG_MDNS_HOSTNAME);

	// Add service to mDNS server
	ESP_ERROR_CHECK( mdns_service_add(NULL, "_device-info", "_udp", CONFIG_UDP_PORT, NULL, 0) );

	// set up address to recvfrom
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(CONFIG_UDP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); /* senderInfo message from ANY */

	// create the socket
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
	LWIP_ASSERT("sock >= 0", sock >= 0);

#if 0
	// set option
	int broadcast=1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
	LWIP_ASSERT("ret >= 0", ret >= 0);
#endif

	// bind socket
	int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	LWIP_ASSERT("ret >= 0", ret >= 0);

	// senderInfo data
	struct sockaddr_in senderInfo;
	socklen_t senderInfoLen = sizeof(senderInfo);
	while(1) {
		char rx_buf[xItemSize];
		int rx_len = recvfrom(sock, rx_buf, sizeof(rx_buf), 0, (struct sockaddr*)&senderInfo, &senderInfoLen);
		ESP_LOGI(TAG, "recvfrom rx_len=%d errno=%d", rx_len, errno);
		if (rx_len < 0) {
			ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
			break;
		}
		if (rx_len > 0) {
			ESP_LOGI(TAG, "recvfrom rx_buf=[%.*s]", rx_len, rx_buf);
			char senderstr[16];
			inet_ntop(AF_INET, &senderInfo.sin_addr, senderstr, sizeof(senderstr));
			ESP_LOGI(TAG, "recvfrom : %s, port=%d", senderstr, ntohs(senderInfo.sin_port));

			size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferRecv );
			ESP_LOGI(TAG, "spacesAvailable=%d", spacesAvailable);
			size_t sended = xMessageBufferSend(xMessageBufferRecv, rx_buf, rx_len, 100);
			if (sended != rx_len)  {
				ESP_LOGE(TAG, "xMessageBufferSend fail rx_len=%d sended=%d", rx_len, sended);
				break;
			}
		}
	} // end while

	// close socket
	close(sock);
	vTaskDelete(NULL);
}

