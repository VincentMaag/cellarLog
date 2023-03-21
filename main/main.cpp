/*

 Testing the si7021 tempsensor


*/

#include <string.h>
#include <string>

#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

// maag components
#include "maag_wifi.h"
#include "maag_gpio.h"
#include "maag_i2c_port.h"
#include "maag_spi_host.h"

// project
#include "cellarLog_projdefs.h"
#include "si7021.h"
#include "sdCard.h"

static const char *TAG = "main";

extern "C" void app_main()
{
    ESP_LOGI(TAG, "STARTING MAIN");
    // =====================================================================
    ESP_LOGI(TAG, "Initializing nvs");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    // ESP_ERROR_CHECK(ret);
    // ESP_LOGI(TAG, "Initializing netif");
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_LOGI(TAG, "Initializing event loop");
    // ESP_ERROR_CHECK( esp_event_loop_create_default() );
    // =====================================================================
    // WIFI
    MaagWifi wifi;
    wifi.setIP("192.168.178.140");
    wifi.setGW("192.168.178.1");
    wifi.setDNS("8.8.8.8");
    // wifi.setSSID("FRITZ!Box 5490 WT");
    wifi.setSSID("ladidaaaaa");
    wifi.setPW("wuesstischwohlgern");
    wifi.init_ap();
    // wifi.init_sta();
    //wifi.createSTAAutoConnectTask(5000, 0);
    // =====================================================================
    // I2C
    // MaagI2CPort i2c;
    // i2c.initPort(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, I2C_MODE_MASTER);
    // SI7021 si7021(i2c.getPort());
    // SD Card
    #define MISO GPIO_NUM_19
    #define MOSI GPIO_NUM_18
    #define CLK GPIO_NUM_5
    #define CS GPIO_NUM_21

    SDCard sdcard;
    //sdcard.okthenfucku();
    sdcard.init(MISO, MOSI, CLK, CS);
    // sdcard.mount();

    //MaagSpiHost myHost;
    //myHost.initHost(SPI2_HOST, MISO, MOSI, CLK);

    while (true)
    {
        
        //si7021.getTemp();

        // ESP_LOGI(TAG, "Temp: %.2f, Humid: %.2f",si7021.getTemp(), si7021.getHumidity());

        ESP_LOGI(TAG, "Attempting to mount...");
        sdcard.mount();
        

        ESP_LOGI(TAG, "Awaiting next loop...");
        vTaskDelay((2000 / portTICK_PERIOD_MS));
    }

    ESP_LOGI(TAG, "FINISHED AND EXITING MAIN");
}
