#pragma once

namespace sjsu::drive {
    // Multiply this with the desired angle to get the correct angle to steer the motors.
    constexpr float angle_correction_factor = 1.3625; // ???? WHY ARE THESE MOTORS SCALED????
    constexpr float k_max_speed = 100;
};