#pragma once

#include <libhal-util/units.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::drive
{

    struct leg_speed_feedback 
    {
        float steer = 5.0f;
        float drive = 5.0f;
    }

    struct motor_speed_feedback
    {

        std::array<leg_speed_feedback, 4> args{};

        // hal::status Print(hal::serial& terminal)
        // {
        //     hal::print<50>(terminal, "\nSteers: left speed: %f right speed: %f back speed: %f Drives: Steers: left speed: %f right speed: %f back speed: %f\n", 
        //     static_cast<float>(left_steer_speed), static_cast<float>(right_steer_speed), static_cast<float>(back_steer_speed),static_cast<float>(left_drive_speed), static_cast<float>(right_drive_speed), static_cast<float>(back_drive_speed));
        //     return hal::success();
        // }
    };

}