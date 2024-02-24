#pragma once
#include "drive_configuration.hpp"
#include "settings.hpp"

namespace sjsu::drive {
/**
 * @brief Validate a given configuration. Only speed needs to validated. It will clamp the speed to between the max speeds.
 * 
 * @note It is possible to have any steering angle or wheel heading, since the steering function is continous.
 * 
 * @param p_config 
 * @return drive_configuration 
 */
inline drive_configuration validate_configuration(drive_configuration p_config)
{
    p_config.wheel_speed = std::clamp(p_config.wheel_speed, -k_max_speed, k_max_speed);

    return p_config;
}
}  // namespace sjsu::drive
