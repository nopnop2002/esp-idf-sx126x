/*	BSD Socket TCP Server

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

#include "lwip/sockets.h"
#include "mdns.h"

static const char *TAG = "TCP-SERVER";

extern MessageBufferHandle_t xMessageBufferRecv;

void tcp_server(void *pvParameters)
{
	ESP_LOGI(TAG, "Start TCP PORT=%d", CONFIG_TCP_PORT);

	// Initialize mDNS
	ESP_ERROR_CHECK( mdns_init() );

	// Set mDNS hostname (required if you want to advertise services)
	ESP_ERROR_CHECK( mdns_hostname_set(CONFIG_MDNS_HOSTNAME) );
	ESP_LOGI(TAG, "mdns hostname set to: [%s]", CONFIG_MDNS_HOSTNAME);

	// Add service to mDNS server
	ESP_ERROR_CHECK( mdns_service_add(NULL, "_device-info", "_tcp", CONFIG_TCP_PORT, NULL, 0) );

	// set up address to recv
	char addr_str[128];
	int addr_family;
	int ip_protocol;
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	//dest_addr.sin_port = htons(PORT);
	dest_addr.sin_port = htons(CONFIG_TCP_PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;
	inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#if 0
	struct sockaddr_in6 dest_addr;
	bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
	dest_addr.sin6_family = AF_INET6;
	dest_addr.sin6_port = htons(CONFIG_TCP_PORT);
	addr_family = AF_INET6;
	ip_protocol = IPPROTO_IPV6;
	inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

	int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	LWIP_ASSERT("listen_sock >= 0", listen_sock >= 0);
	ESP_LOGI(TAG, "Socket created");

	int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	LWIP_ASSERT("err == 0", err == 0);
	ESP_LOGI(TAG, "Socket bound, port %d", CONFIG_TCP_PORT);

	err = listen(listen_sock, 1);
	LWIP_ASSERT("err == 0", err == 0);
	ESP_LOGI(TAG, "Socket listening");

	bool running = true;
	while (running) {
		// Wait for connect
		ESP_LOGI(TAG, "Waiting for connect");
		struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		socklen_t addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0) {
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG, "Socket accepted");

		while (1) {
			char rx_buf[256]; // Maximum Payload size of SX1261/62/68 is 255
			int rx_len = recv(sock, rx_buf, sizeof(rx_buf), 0);
			ESP_LOGI(TAG, "recv rx_len=%d errno=%d", rx_len, errno);

			// Error occurred during receiving
			if (rx_len < 0) {
				ESP_LOGE(TAG, "recv failed: errno %d", errno);
				break;
			}
			// Connection closed by client
			else if (rx_len == 0) {
				ESP_LOGI(TAG, "Connection closed by client");
				break;
			}
			// Data received
			else {
				// Get the sender's ip address as string
				if (source_addr.sin6_family == PF_INET) {
					inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
				} else if (source_addr.sin6_family == PF_INET6) {
					inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
				}
				ESP_LOGI(TAG, "Received %d bytes from %s:", rx_len, addr_str);
				ESP_LOGI(TAG, "[%.*s]", rx_len, rx_buf);

				size_t spacesAvailable = xMessageBufferSpacesAvailable( xMessageBufferRecv );
				ESP_LOGI(TAG, "spacesAvailable=%d", spacesAvailable);
				size_t sended = xMessageBufferSend(xMessageBufferRecv, rx_buf, rx_len, 100);
				if (sended != rx_len)  {
					ESP_LOGE(TAG, "xMessageBufferSend fail rx_len=%d sended=%d", rx_len, sended);
					running = false;
					break;
				}

			}
		} // end while

		// Close socket.
		ESP_LOGI(TAG, "Close socket");
		close(sock);

	} // end while

	vTaskDelete(NULL);
}
