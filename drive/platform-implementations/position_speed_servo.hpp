#pragma once 

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::drive {

class position_speed_servo  {
public:
    inline hal::status position_speed(hal::degrees p_position, hal::rpm p_speed) {
        HAL_CHECK(driver_position_speed(p_position, p_speed));
        return hal::success();
    }
private:

    virtual hal::status driver_position_speed(hal::degrees p_position, hal::rpm p_speed) = 0;
};
}


