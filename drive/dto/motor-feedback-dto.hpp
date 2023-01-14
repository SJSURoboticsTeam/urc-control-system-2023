#pragma once

#include <libhal/units.hpp>
#include <libhal-util/serial.hpp>

namespace Drive
{

    struct motor_feedback
    {
        hal::rpm left_steer_speed = 5_rpm;
        hal::rpm right_steer_speed = 5_rpm;
        hal::rpm back_steer_speed = 5_rpm;

        void print(hal::serial& terminal)
        {
            HAL_CHECK(hal::write(terminal, "left steer speed: %d", static_cast<int>(left_steer_speed)));
            HAL_CHECK(hal::write(terminal, "right steer speed: %d", static_cast<int>(right_steer_speed)));
            HAL_CHECK(hal::write(terminal, "back steer speed: %d", static_cast<int>(back_steer_speed)));
        }
    };

}