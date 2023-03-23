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
#include "maag_sntp.h"

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
    // =====================================================================
    // WIFI
    MaagWifi wifi;
    wifi.setIP("192.168.178.140");
    wifi.setGW("192.168.178.1");
    wifi.setDNS("8.8.8.8");
    wifi.setSSID("FRITZ!Box 7583 AE 2.4 Ghz");
    // wifi.setSSID("ladidaaaaa");
    wifi.setPW("72176317897889201379");
    // wifi.init_ap();
    wifi.init_sta();
    wifi.createSTAAutoConnectTask(5000, 0);
    // =====================================================================
    // SNTP (online clock)
    //
    MaagSNTP sntp;
    sntp.setSynchInterval(5 * 60 * 1000);
	sntp.initStart();
    // =====================================================================
    // I2C
    MaagI2CPort i2c;
    i2c.initPort(I2C_NUM_0, GPIO_NUM_25, GPIO_NUM_26, I2C_MODE_MASTER);
    SI7021 si7021(i2c.getPort());
    // =====================================================================
    // SD Card
    SDCard sdcard;
    sdcard.mount_sdmmc(); // 15 = CMD = pin2, 14 = clk = pin5, 2 = D0 = pin7

    // set timezone
    setenv("TZ", "MEZ-1MESZ", 1);
	tzset();

    while (true)
    {
        // get local time
	    time_t now_as_time_t = time(0);
        struct tm now_as_tm = {};
		localtime_r(&now_as_time_t, &now_as_tm);
        // build timestamp string as desired
        now_as_tm.tm_mon = now_as_tm.tm_mon + 1;
        now_as_tm.tm_year = now_as_tm.tm_year + 1900;
        char timestamp[72];
        sprintf(timestamp, "%i.%i.%i %i:%i:%i", now_as_tm.tm_mday,now_as_tm.tm_mon,now_as_tm.tm_year,now_as_tm.tm_hour,now_as_tm.tm_min,now_as_tm.tm_sec);

        //ESP_LOGI(TAG, "Timestamp: %s",timestamp);
        // ESP_LOGI(TAG, "Temp: %.2f, Humid: %.2f",si7021.getTemp(), si7021.getHumidity());

        logData_t t;
        t.timestamp = timestamp;
        t.value1 = si7021.getTemp();
        vTaskDelay((100 / portTICK_PERIOD_MS));
        t.value2 = si7021.getHumidity();

        sdcard.log(t);

        ESP_LOGI(TAG, "Awaiting next loop...");
        vTaskDelay((30*1000 / portTICK_PERIOD_MS));
    }

    ESP_LOGI(TAG, "FINISHED AND EXITING MAIN");
}
