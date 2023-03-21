/*
    ...

*/
#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "esp_err.h"
#include <string>
#include "driver/gpio.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"


typedef struct {
    float value1;
    float value2;
    float value3;
    float value4;
    std::string timestamp;
}logData_t;

class SDCard
{
private:
    
    

    std::string m_filename;
    logData_t m_logData;

    esp_vfs_fat_sdmmc_mount_config_t m_mount_config;
    sdmmc_card_t *m_card;
    sdmmc_host_t m_host;
    spi_bus_config_t m_bus_cfg = {};
    sdspi_device_config_t m_slot_config;
    

public:
    SDCard();

    void init(gpio_num_t miso_, gpio_num_t mosi_, gpio_num_t sck_, gpio_num_t cs_);

    esp_err_t mount();

    esp_err_t unmount();

    void okthenfucku();

    // esp_err_t setFileName(std::string filename_);

    // esp_err_t log(logData_t newData);

};



#endif /* __SDCARD_H__ */