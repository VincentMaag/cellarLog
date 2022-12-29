/*
    ...

*/
#ifndef __UART_H__
#define __UART_H__


class Uart
{
private:

    static void uartEchoTask(void *arg);

public:

    // create and setup uart object
    Uart(gpio_num_t gpio_rx_, gpio_num_t gpio_tx_);

    // create and start uart task
    esp_err_t createEchoTask();

    
};

// Uart::Uart(/* args */)
// {
// }

// Uart::~Uart()
// {
// }




#endif /* __UART_H__ */