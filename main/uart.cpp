/*
	...


*/

#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <string>

#include "uart.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

#define UART_PORT UART_NUM_2

int num = 0;

// static TAG
static const char *TAG = "uart";

// =============================================================================================================
// CLASS UART
// =============================================================================================================

// create an init serial in constructor

Uart::Uart()
{

	uart_config_t uart_config = {};
	uart_config.baud_rate = 115200;
	uart_config.data_bits = UART_DATA_8_BITS;
	uart_config.parity = UART_PARITY_DISABLE;
	uart_config.stop_bits = UART_STOP_BITS_1;
	uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	uart_config.source_clk = UART_SCLK_APB;

	// We won't use a buffer for sending data.
	ESP_ERROR_CHECK(uart_driver_install(UART_PORT, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	ESP_LOGW(TAG, "Uart created and configured");
}

esp_err_t Uart::createEchoTask()
{
	static uint8_t init = 0;

	if (init)
	{
		ESP_LOGW(TAG, "Uart task ALREADY created");
		return ESP_FAIL;
	}

	xTaskCreate(Uart::uartEchoTask, "uart_echo_task", 2048, NULL, 10, NULL);
	ESP_LOGW(TAG, "Uart task created");
	return ESP_OK;
}

void Uart::uartEchoTask(void *arg)
{

	const static char *TAG2 = "uart_task";

	// Configure a temporary buffer for the incoming data
	uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE);

	char answerString[] = ": ESP answering! \r\n";

	uint8_t dataOrig[6] = {0, 0, 0, 4, 5, 6};

	uint8_t dataAppend[1] = {65}; // char "A"

	std::string answerStringCpp = "ESP answering, but in Cpp LF";

	while (1)
	{
		// clear all
		memset(data, 0, RX_BUF_SIZE);
		// Read data from the UART
		int lengthSend = 0;
		int lengthIn = uart_read_bytes(UART_PORT, data, RX_BUF_SIZE, 20 / portTICK_PERIOD_MS);
		// wait until we receive something
		if (lengthIn > 0)
		{
			// Append some stuff to input
			// memcpy((void*)data[lengthIn], (const void*)dataAppend, 3);
			
			//ESP_LOGE(TAG, "before memcpy: [0]: %i, [1]: %i, [2]: %i, [3]: %i, input length: %i",data[0],data[1],data[2],data[3],lengthIn);

			// we need to get rid of the "Carriage Feed" (dez 13) when sending back, so that our appended message is received!
			memcpy((void *)&(data[lengthIn-1]), (const void *)answerString, sizeof(answerString));
			
			//ESP_LOGE(TAG, "after memcpy: [0]: %i, [1]: %i, [2]: %i, [3]: %i, ===[4]: %i===",data[0],data[1],data[2],data[3],data[4]);


			//ESP_LOGW(TAG, "memcpy successfull");
			lengthSend = lengthIn + sizeof(answerString);
			// Write data back to the UART.
			//int lengthOut = uart_write_bytes(UART_PORT, (const char *)data, lengthSend);
			int lengthOut = uart_write_bytes(UART_PORT, (const char *)data, lengthSend);
			if (lengthOut > 0)
			{
				ESP_LOGW(TAG, "esp received stuff and is sending more stuff back");
			}
		}

		// vTaskDelay((1000 / portTICK_PERIOD_MS));
	}
}
