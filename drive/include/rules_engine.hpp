#pragma once
#include "drive_configuration.hpp"
#include "settings.hpp"

namespace sjsu::drive {
inline drive_configuration validate_configuration(drive_configuration p_config)
{
    p_config.wheel_speed = std::clamp(p_config.wheel_speed, -k_max_speed, k_max_speed);

    return p_config;
}
}  // namespace sjsu::drive
