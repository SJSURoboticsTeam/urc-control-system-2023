#pragma once

#include "drive_configuration.hpp"
namespace sjsu::drive {
    // Multiply this with the desired angle to get the correct angle to steer the motors.
    constexpr float angle_correction_factor = 1.3625; // ???? WHY ARE THESE MOTORS SCALED????
    
    constexpr float k_max_speed = 100;

    // These control how sensitive the system is to changes in the target.
    drive_configuration config_kP = {
        .steering_angle = 1,
        .wheel_heading = 10,
        .wheel_speed = 20,
    };

    // These control how fast the system can react to changes in the target.
    drive_configuration config_max_delta = {
        .steering_angle = 40,
        .wheel_heading = 360,
        .wheel_speed = 50,
    };

};