/*
    ...

*/
#ifndef WS2812_H__
#define WS2812_H__

#include "driver/rmt.h"

#define T0H 14  // 0 bit high time
#define T1H 52  // 1 bit high time
#define TL  52  // low time for either bit
#define BITS_PER_LED_CMD 24 
#define NUM_LEDS 280
#define LED_BUFFER_ITEMS ((NUM_LEDS * BITS_PER_LED_CMD))

#define RED   0x00FF00
#define GREEN 0xFF0000
#define BLUE  0x0000FF
#define WHITE 0xFFFFFF
#define WARMWHITE_1 0xc0ef70
#define WARMWHITE_2 0x70e425

//

#define NOCOLOR 0x000000

class WS2812
{
private:
    /* data */
    rmt_channel_t m_rmtChannel;
    gpio_num_t m_gpio;
    uint32_t m_nrOfLED;
    // pointer that will point to our buffer, once initialized
    uint32_t m_nrOfRmtItems;
    rmt_item32_t *pm_DataBuffer;
    // setup rmt buffer. Pass an array of led-colors and how many of those we want to use (can be less than whole array passed)
    esp_err_t loadRmtBuffer(uint32_t* pData_, uint32_t length_);

public:
    WS2812(/* args */);
    // init RMT channel and assign GPIO. Set maximum allowed leds
    esp_err_t initChannel(rmt_channel_t channel_, gpio_num_t gpio_, uint32_t nrOfLED_);
    // write data array to rmt output
    esp_err_t writeLEDs(uint32_t* pData_, uint32_t nrOfLEDs_);

};



#endif /* WS2812_H__ */