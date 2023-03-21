/*
	...
	ToDo: check for NULL passings of ports etc.!

*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "si7021.h"

#define SI7021_ADDR 0x40 //!< I2C address

#define SI7021_CMD_HUM 0xE5
#define SI7021_CMD_HUM_NHMM 0xF5
#define SI7021_CMD_TEMP 0xE3
#define SI7021_CMD_TEMP_NHMM 0xF3

// static TAG
static const char *TAG = "si7021";

// =============================================================================================================
// CLASS SI7021
// =============================================================================================================

// in the constructor we connect the device with its port and set the device address
SI7021::SI7021(i2c_port_t port_)
{
	MaagI2CDevice::setPort(port_);
	MaagI2CDevice::setDeviceAddress(SI7021_ADDR);
	ESP_LOGW(TAG, "SI7021 instance created");
}

float SI7021::getTemp(){

	uint8_t temp_addr = SI7021_CMD_TEMP;
	uint8_t read_data[2] = {};				// si7021 returns 2 bytes

	if(MaagI2CDevice::readClockStretched(&temp_addr, 1, &read_data[0], 2, 100) != ESP_OK){
		return 42.42f;
	}

	uint16_t raw = (read_data[0] << 8) | read_data[1];
	float temp = 175.72f * (float)raw / 65536 - 46.85f;

	return temp;
}

float SI7021::getHumidity(){

	uint8_t temp_addr = SI7021_CMD_HUM;
	uint8_t read_data[2] = {};				// si7021 returns 2 bytes

	if(MaagI2CDevice::readClockStretched(&temp_addr, 1, &read_data[0], 2, 100) != ESP_OK){
		return 24.24f;
	}
	
	uint16_t raw = (read_data[0] << 8) | read_data[1];
	float hum = 125.0f * (float)raw / 65536 - 6.0f;
	
	return hum;
}