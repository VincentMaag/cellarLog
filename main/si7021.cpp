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

	// prepare i2c communication and attempt to read
	uint8_t temp_addr = SI7021_CMD_TEMP; 	// address of temperature
	uint8_t read_data[2] = {};				// si7021 returns 2 bytes
	// if reading is not successfull, return silly value. As the device uses clock stretching, we need to wait for response
	if(MaagI2CDevice::readClockStretched(&temp_addr, 1, &read_data[0], 2, 100) != ESP_OK){
		return 42.42f;
	}
	ESP_LOGW(TAG, "SI7021 Raw data read: [0]: %i, [1]: %i", read_data[0], read_data[1]);
	
	// first read byte is MSB, second is LSB (datasheet)
	uint16_t raw = (read_data[0] << 8) | read_data[1];
	// ESP_LOGW(TAG, "converted to 16bit:: %i", raw);
	
	// attempt to convert to °C, see datasheet
	float temp = 175.72f * (float)raw / 65536 - 46.85f;

	// ESP_LOGW(TAG, "converted to float: %.2f", temp);

	return 0.0f;

}