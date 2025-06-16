/*	USB CDC-ACM Virtual COM Port example

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "freertos/semphr.h"

#include "usb/cdc_acm_host.h"
#include "usb/vcp_ch34x.hpp"
#include "usb/vcp_cp210x.hpp"
#include "usb/vcp_ftdi.hpp"
#include "usb/vcp.hpp"
#include "usb/usb_host.h"

extern MessageBufferHandle_t xMessageBufferTx;
extern MessageBufferHandle_t xMessageBufferRx;
extern size_t xItemSize;

using namespace esp_usb;

// Change these values to match your needs
//#define EXAMPLE_BAUDRATE	 (115200)
//#define EXAMPLE_STOP_BITS	 (0) // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
//#define EXAMPLE_PARITY	 (0) // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
//#define EXAMPLE_DATA_BITS	 (8)
#define EXAMPLE_BAUDRATE	 CONFIG_VCP_BAUDRATE
#define EXAMPLE_STOP_BITS	 CONFIG_VCP_STOP_BITS
#define EXAMPLE_PARITY		 CONFIG_VCP_PARITY
#define EXAMPLE_DATA_BITS	 CONFIG_VCP_DATA_BITS

namespace {
static const char *TAG = "VCP";
static EventGroupHandle_t device_connected_group;

#define VCP_CONNECTED_BIT BIT0

/**
 * @brief Data received callback
 *
 * Just pass received data to stdout
 *
 * @param[in] data	   Pointer to received data
 * @param[in] data_len Length of received data in bytes
 * @param[in] arg	   Argument we passed to the device open function
 * @return
 *	 true:	We have processed the received data
 *	 false: We expect more data
 */
static bool handle_rx(const uint8_t *data, size_t data_len, void *arg)
{
#if CONFIG_SENDER
	//printf("%.*s", data_len, data);
	ESP_LOGI(TAG, "Receiving data through CdcAcmDevice");
	ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);

	// The VCP termination character is CR+LF. So Remove CR/LF.
	size_t _data_len = 0;
	for (int i=0;i<data_len;i++) {
		if (data[i] == 0x0d) continue;
		if (data[i] == 0x0a) continue;
		_data_len++;
	}

	// Send to radio task
	char buffer[xItemSize];
	if (_data_len > xItemSize) _data_len = xItemSize;
	memcpy(buffer, (char *)data, _data_len);
	size_t sended = xMessageBufferSendFromISR(xMessageBufferRx, buffer, _data_len, NULL);
	if (sended != _data_len) {
		ESP_LOGE(__FUNCTION__, "xMessageBufferSendFromISR Fail");
	}
#endif
	return true;
}

/**
 * @brief Device event callback
 *
 * Apart from handling device disconnection it doesn't do anything useful
 *
 * @param[in] event    Device event type and data
 * @param[in] user_ctx Argument we passed to the device open function
 */
static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
{
	switch (event->type) {
	case CDC_ACM_HOST_ERROR:
		ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %d", event->data.error);
		break;
	case CDC_ACM_HOST_DEVICE_DISCONNECTED:
		ESP_LOGI(TAG, "Device suddenly disconnected");
		xEventGroupClearBits(device_connected_group, VCP_CONNECTED_BIT);
		break;
	case CDC_ACM_HOST_SERIAL_STATE:
		ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
		break;
	case CDC_ACM_HOST_NETWORK_CONNECTION:
	default: break;
	}
}

/**
 * @brief USB Host library handling task
 *
 * @param arg Unused
 */
void usb_lib_task(void *arg)
{
	while (1) {
		// Start handling system events
		uint32_t event_flags;
		usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
		ESP_LOGD(pcTaskGetName(NULL), "usb_host_lib_handle_events event_flags=%lu", event_flags);
		if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
			ESP_LOGI(pcTaskGetName(NULL), "USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS");
			ESP_ERROR_CHECK(usb_host_device_free_all());
		}
		if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
			ESP_LOGI(pcTaskGetName(NULL), "USB: All devices freed");
			// Continue handling USB events to allow device reconnection
		}
	}
}
}

/**
 * @brief Main application
 *
 * This function shows how you can use Virtual COM Port drivers
 */
extern "C" void cdc_acm_vcp_task(void *pvParameters)
{
	device_connected_group = xEventGroupCreate();
	xEventGroupClearBits(device_connected_group, VCP_CONNECTED_BIT);

	// Install USB Host driver. Should only be called once in entire application
	ESP_LOGI(TAG, "Installing USB Host");
	usb_host_config_t host_config = {};
	host_config.skip_phy_setup = false;
	host_config.intr_flags = ESP_INTR_FLAG_LEVEL1;
	ESP_ERROR_CHECK(usb_host_install(&host_config));

	// Create a task that will handle USB library events
	BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, NULL);
	assert(task_created == pdTRUE);

	ESP_LOGI(TAG, "Installing CDC-ACM driver");
	ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

	// Register VCP drivers to VCP service
	VCP::register_driver<FT23x>();
	VCP::register_driver<CP210x>();
	VCP::register_driver<CH34x>();

	// Do everything else in a loop, so we can demonstrate USB device reconnections
	while (true) {
		const cdc_acm_host_device_config_t dev_config = {
			.connection_timeout_ms = 5000, // 5 seconds, enough time to plug the device in or experiment with timeout
			.out_buffer_size = 512,
			.in_buffer_size = 512,
			.event_cb = handle_event,
			.data_cb = handle_rx,
			.user_arg = NULL,
		};

		// You don't need to know the device's VID and PID. Just plug in any device and the VCP service will load correct (already registered) driver for the device
		ESP_LOGI(TAG, "Opening any VCP device...");
		auto vcp = std::unique_ptr<CdcAcmDevice>(VCP::open(&dev_config));

		if (vcp == nullptr) {
			ESP_LOGW(TAG, "Failed to open VCP device");
			continue;
		}
		vTaskDelay(10);

		ESP_LOGI(TAG, "Setting up line coding");
		xEventGroupSetBits(device_connected_group, VCP_CONNECTED_BIT);
		cdc_acm_line_coding_t line_coding = {
			.dwDTERate = EXAMPLE_BAUDRATE,
			.bCharFormat = EXAMPLE_STOP_BITS,
			.bParityType = EXAMPLE_PARITY,
			.bDataBits = EXAMPLE_DATA_BITS,
		};
		ESP_ERROR_CHECK(vcp->line_coding_set(&line_coding));

		/*
		Now the USB-to-UART converter is configured and receiving data.
		You can use standard CDC-ACM API to interact with it. E.g.

		ESP_ERROR_CHECK(vcp->set_control_line_state(false, true));
		ESP_ERROR_CHECK(vcp->tx_blocking((uint8_t *)"Test string", 12));
		*/

		// We are done. Wait for device disconnection and start over
		ESP_ERROR_CHECK(vcp->set_control_line_state(true, true));
		ESP_LOGI(TAG, "Done. You can reconnect the VCP device to run again.");

		// Receive from radio task
		char buffer[xItemSize];
		while(1) {
			size_t received = xMessageBufferReceive(xMessageBufferTx, buffer, sizeof(buffer), 100);
			ESP_LOGD(TAG, "xMessageBufferReceive received=%d", received);
			if (received > 0) {
				ESP_LOGI(TAG, "Sending data through CdcAcmDevice");
				ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, received, ESP_LOG_INFO);
				if (buffer[received-1] != 0x0a) {
					buffer[received] = 0x0a;
					received++;
					ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, received, ESP_LOG_INFO);
				}
				ESP_ERROR_CHECK(vcp->tx_blocking((uint8_t*)buffer, received));
			}
			EventBits_t connected = xEventGroupGetBits(device_connected_group);
			ESP_LOGD(TAG, "connected=0x%lx", connected);
			if (connected == 0) break;
		}
	} // end while

	// Never reach here
	vTaskDelete(NULL);
}
