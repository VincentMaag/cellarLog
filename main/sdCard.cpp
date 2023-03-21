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
#define SPI_DMA_CHAN    1

static const char *TAG = "sdCard";

// =============================================================================================================
// CLASS SDCARD
// =============================================================================================================
SDCard::SDCard(){
	ESP_LOGW(TAG, "SDCard instance created");
}

void SDCard::okthenfucku(){

    esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_NUM_18,
        .miso_io_num = GPIO_NUM_19,
        .sclk_io_num = GPIO_NUM_5,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = GPIO_NUM_21;
    slot_config.host_id = SPI2_HOST;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);


}

void SDCard::init(gpio_num_t miso_, gpio_num_t mosi_, gpio_num_t sck_, gpio_num_t cs_){

	ESP_LOGI(TAG, "Initializing SD card, using SPI peripheral");


	m_mount_config.format_if_mount_failed = true;
    m_mount_config.max_files = 5;
    m_mount_config.allocation_unit_size = 16 * 1024;

    m_host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

 	m_host = SDSPI_HOST_DEFAULT();
    m_host.flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG;
    m_host.slot = SPI2_HOST;
    m_host.max_freq_khz = 20000;
    m_host.io_voltage = 3.3f;
    m_host.init = &sdspi_host_init;
    m_host.set_bus_width = NULL;
    m_host.get_bus_width = NULL;
    m_host.set_bus_ddr_mode = NULL;
    m_host.set_card_clk = &sdspi_host_set_card_clk;
    m_host.do_transaction = &sdspi_host_do_transaction;
    m_host.deinit_p = &sdspi_host_remove_device;
    m_host.io_int_enable = &sdspi_host_io_int_enable;
    m_host.io_int_wait = &sdspi_host_io_int_wait;
    m_host.command_timeout_ms = 0;

	m_bus_cfg.mosi_io_num = mosi_; 
    m_bus_cfg.miso_io_num = miso_;
    m_bus_cfg.sclk_io_num = sck_;
    m_bus_cfg.quadwp_io_num = -1;
    m_bus_cfg.quadhd_io_num = -1;
    m_bus_cfg.max_transfer_sz = 4000;

	esp_err_t ret = spi_bus_initialize(SPI2_HOST, &m_bus_cfg, SPI_DMA_DISABLED);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    //m_slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
	m_slot_config.host_id = (spi_host_device_t)m_host.slot;
	m_slot_config.gpio_cs = cs_;
	m_slot_config.gpio_cd = GPIO_NUM_NC;
	m_slot_config.gpio_wp = GPIO_NUM_NC;
	m_slot_config.gpio_int = GPIO_NUM_NC;
    



    // m_card;

}

esp_err_t SDCard::mount(){

	 //esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &m_host, &m_slot_config, &m_mount_config, &m_card);

	esp_err_t ret = esp_vfs_fat_sdspi_mount("/sdcard", &m_host, &m_slot_config, &m_mount_config, &m_card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set format_if_mount_failed = true.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return ret;
    }
	ESP_LOGI(TAG, "Filesystem mounted successfully");
    //sdmmc_card_print_info(stdout, m_card);
    return ret;
}

esp_err_t SDCard::unmount(){
    return esp_vfs_fat_sdmmc_unmount();
}