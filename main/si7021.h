/*
    ...

*/
#ifndef __SI7021_H__
#define __SI7021_H__


#include "maag_i2c_device.h"

class SI7021: public MaagI2CDevice
{
private:
    /* data */
public:
    SI7021(i2c_port_t port_);
    // get temperature of sensor in °C. Returns 42.42° if something goes wrong
    float getTemp();

};



#endif /* __SI7021_H__ */