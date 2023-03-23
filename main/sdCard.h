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
    
    std::string m_filename = "/cellar.csv";
    logData_t m_logData;

    esp_vfs_fat_sdmmc_mount_config_t m_mount_config;
    sdmmc_card_t *m_card;
    sdmmc_host_t m_host;
    sdmmc_slot_config_t m_slot_config;

    uint8_t m_mountedState;

public:
    SDCard();

    // void init_sdspi(gpio_num_t miso_, gpio_num_t mosi_, gpio_num_t sck_, gpio_num_t cs_);

    // setup sd card in sdmmc mode. For now, Only 1-bit mode and fixed clk frequency is supported. Using sdmmc slot 1
    esp_err_t mount_sdmmc();

    esp_err_t unmount_sdmmc();

    void log(logData_t newData_);

    void writeSomething();


    //esp_err_t setFileName(std::string filename_);

    

};



#endif /* __SDCARD_H__ */