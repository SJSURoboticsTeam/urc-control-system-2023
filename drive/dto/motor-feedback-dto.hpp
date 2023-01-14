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
            HAL_CHECK(hal::write(terminal, "left steer speed:" + static_cast<int>(left_steer_speed)));
            HAL_CHECK(hal::write(terminal, "right steer speed:" + static_cast<int>(right_steer_speed)));
            HAL_CHECK(hal::write(terminal, "back steer speed: " + static_cast<int>(back_steer_speed)));
            return hal::success();
        }
    };

}