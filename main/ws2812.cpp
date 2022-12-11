/*
	...


*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "ws2812.h"

// static TAG
static const char *TAG = "ws2812";

// =============================================================================================================
// CLASS WS2812
// =============================================================================================================

WS2812::WS2812(/* args */)
{
	ESP_LOGW(TAG, "WS2812 instance created");
}



esp_err_t WS2812::initChannel(rmt_channel_t channel_, gpio_num_t gpio_, uint32_t nrOfLED_)
{
	rmt_config_t config;
	config.rmt_mode = RMT_MODE_TX;
	config.channel = channel_;
	config.gpio_num = gpio_;
	config.mem_block_num = 3;
	config.tx_config.loop_en = false;
	config.tx_config.carrier_en = false;
	config.tx_config.idle_output_en = true;
	config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	config.clk_div = 2;

	ESP_ERROR_CHECK(rmt_config(&config));
	ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

	m_rmtChannel = channel_;
	m_gpio = gpio_;
	m_nrOfLED = nrOfLED_;
	m_nrOfRmtItems = nrOfLED_ * BITS_PER_LED_CMD;
	
	// allocate memory for the buffer on the heap. Nr of LED * Bits per LED needed. Every bit is an rmt item!
	pm_DataBuffer = new rmt_item32_t[m_nrOfRmtItems];

	return ESP_OK;
}

esp_err_t WS2812::loadRmtBuffer(uint32_t *pData_, uint32_t length_)
{
	if(length_ > m_nrOfLED){
		ESP_LOGE(TAG, "Too many LEDs written");
		return ESP_FAIL;
	}
	for (uint32_t led = 0; led < length_; led++)
	{
		uint32_t bits_to_send = pData_[led];
		uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);
		for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++)
		{
			uint32_t bit_is_set = bits_to_send & mask;
			pm_DataBuffer[led * BITS_PER_LED_CMD + bit] = bit_is_set ? (rmt_item32_t){{{T1H, 1, TL, 0}}} : (rmt_item32_t){{{T0H, 1, TL, 0}}};
			mask >>= 1;
		}
	}
	return ESP_OK;
}

esp_err_t WS2812::writeLEDs(uint32_t *pData_, uint32_t nrOfLEDs_)
{
	WS2812::loadRmtBuffer(pData_, nrOfLEDs_);
	ESP_ERROR_CHECK(rmt_write_items(m_rmtChannel, pm_DataBuffer, m_nrOfRmtItems, false));
	ESP_ERROR_CHECK(rmt_wait_tx_done(m_rmtChannel, portMAX_DELAY));

	return ESP_OK;
}



