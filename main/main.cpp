/*
neopixel project. Todos:

- remove components stuff that will not be needed
- comment out wifi (we'll get to that later)
- get simple project to compile

- include old legacy code and let leds glow

- make a rmt class --> component in maag-components
- make an led-class (2812 or what its called)


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

// project
#include "neopixel_projdefs.h"
#include "testClass.h"
#include "ws2812.h"

static const char *TAG = "main";

extern "C" void app_main()
{
    ESP_LOGI(TAG, "STARTING MAIN");
    // =====================================================================
    // global initializing, objects, parameters, nvs, etc.
    //
    ESP_LOGI(TAG, "Initializing nvs");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // not sure why i added next lines...
    // ESP_LOGI(TAG, "Initializing netif");
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_LOGI(TAG, "Initializing event loop");
    // ESP_ERROR_CHECK( esp_event_loop_create_default() );
    // =====================================================================
    // Wifi object
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
    // webserver object
    // NixieWebserver webserver;  // create webserver object
    // webserver.createServer(0); // start webserver --> create freRtos tasks
    // =====================================================================
    // GPIO's --> just testcode
    // GpioInput gpioIn(GPIO_NUM_14, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE);
    // GpioOutput gpioOut(GPIO_NUM_5, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE);
    // gpioOut.setOutput(true);
    // gpio2.setOutput(true);
    // =====================================================================
    // SPI
    // create a spi host with miso, mosi, clk
    // MaagSpiHost spi;
    // spi.initHost(SPI2_HOST, GPIO_NUM_19, GPIO_NUM_23, GPIO_NUM_18);
    // // create an hv5622 spi device
    // NixieHv5622 hv5622;
    // // init the device and connect to a host. Set clk frequency
    // hv5622.initDevice(spi.getHostDevice(), 10000, GPIO_NUM_5);
    // // initialize gpios needed for hv5622 coomunication
    // hv5622.initGpios(GPIO_NUM_20, GPIO_NUM_21);

    // =====================================================================
    // LED
    WS2812 ws2812;
    ws2812.initChannel(RMT_CHANNEL_0,GPIO_NUM_13,50);
    uint32_t ui32colors[NUM_LEDS] = {};
    for (size_t i = 0; i < 50; i++)
    {
        /* code */
        ui32colors[i] = NOCOLOR;
    }
    // clear all colors first
    ws2812.writeLEDs(ui32colors,50);
    vTaskDelay((1000 / portTICK_PERIOD_MS));
    

    // TickType_t previousWakeTime = xTaskGetTickCount();
    while (true)
    {
        ui32colors[0] = RED;
        ui32colors[1] = GREEN;
        ui32colors[2] = BLUE;
        ui32colors[6] = RED;
        ui32colors[7] = GREEN;
        ui32colors[8] = BLUE;
        ws2812.writeLEDs(ui32colors,9);


        ESP_LOGI(TAG, "Main Looping");
        // xTaskDelayUntil(&previousWakeTime,(pMaagWifi->m_autoConnectTasTicksToDelay / portTICK_PERIOD_MS));
        vTaskDelay((5000 / portTICK_PERIOD_MS));
    }

    ESP_LOGI(TAG, "FINISHED AND EXITING MAIN");
}
