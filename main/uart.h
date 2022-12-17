/*
    ...

*/
#ifndef __UART_H__
#define __UART_H__


class Uart
{
private:
    /* data */
public:
    Uart(/* args */);
    ~Uart();

    esp_err_t createEchoTask();

    static void uartEchoTask(void *arg);
};

// Uart::Uart(/* args */)
// {
// }

// Uart::~Uart()
// {
// }




#endif /* __UART_H__ */