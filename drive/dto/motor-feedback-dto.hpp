#pragma once

#include <libhal-util/units.hpp>
#include <libhal-util/serial.hpp>

namespace Drive
{
    struct motor_feedback
    {
        hal::rpm left_steer_speed = hal::rpm(5);
        hal::rpm right_steer_speed = hal::rpm(5);
        hal::rpm back_steer_speed = hal::rpm(5);

        hal::status print(hal::serial& terminal)
        {
            HAL_CHECK(hal::print<50>(terminal, "left speed: %f right speed: %f back speed: %f\n", 
            static_cast<float>(left_steer_speed), static_cast<float>(right_steer_speed), static_cast<float>(back_steer_speed)));
            return hal::success();
        }
    };

}