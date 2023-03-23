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
#include "esp_log.h"
// #include "esp_vfs_fat.h"
// #include "driver/sdmmc_host.h"
// #include "driver/sdspi_host.h"
// #include "sdmmc_cmd.h"

#include "sdCard.h"

#define MOUNT_POINT "/sdcard"

static const char *TAG = "sdCard";

// =============================================================================================================
// CLASS SDCARD
// =============================================================================================================
SDCard::SDCard(){
	ESP_LOGW(TAG, "SDCard instance created");
}

esp_err_t SDCard::mount_sdmmc(){

    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initializing SD card, using SDMMC peripheral");

    m_mount_config = {};
    m_mount_config.format_if_mount_failed = true;
    m_mount_config.max_files = 5;
    m_mount_config.allocation_unit_size = 16 * 1024;

    // m_card;

    const char mount_point[] = MOUNT_POINT;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT; //SDMMC_FREQ_PROBING
    m_host = host;
    
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    m_slot_config = slot_config;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &m_host, &m_slot_config, &m_mount_config, &m_card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        m_mountedState = 0;
        return ret;
    }
    m_mountedState = 1;
    ESP_LOGI(TAG, "Filesystem mounted");

    sdmmc_card_print_info(stdout, m_card);

    return ret;
}

esp_err_t SDCard::unmount_sdmmc(){

    esp_err_t ret = ESP_OK;
    const char mount_point[] = MOUNT_POINT;
    ret = esp_vfs_fat_sdcard_unmount(mount_point, m_card);
    ESP_LOGI(TAG, "Card unmounted");
    m_mountedState = 0;
    return ret;
}

void SDCard::log(logData_t newData_){

    if (!m_mountedState){
        mount_sdmmc();
        return;
    }

    char filename[30];
    sprintf(filename, "%s%s", MOUNT_POINT, m_filename.c_str());

    ESP_LOGI(TAG, "Opening file %s", filename);
    FILE *f = fopen(filename, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        SDCard::unmount_sdmmc();
        m_mountedState = 0;
        return;
    }
    fprintf(f, "%s;%.2f;%.2f;%.2f;%.2f\n", newData_.timestamp.c_str(),newData_.value1, newData_.value2, newData_.value3, newData_.value4);
    fclose(f);
    ESP_LOGI(TAG, "File written");

}

void SDCard::writeSomething(){

    char filename[30];

    sprintf(filename, "%s%s", MOUNT_POINT, m_filename.c_str());

    // First create a file.
    // const char *file_hello = MOUNT_POINT"/hello.csv";

    ESP_LOGI(TAG, "Opening file %s", filename);
    FILE *f = fopen(filename, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "%.2f;%.2f\n", 0.123f,42.42f);
    fclose(f);
    ESP_LOGI(TAG, "File written");

}


