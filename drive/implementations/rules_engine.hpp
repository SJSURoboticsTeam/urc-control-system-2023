#pragma once

#include "../include/mission_control.hpp"
#include "../include/settings.hpp"
namespace sjsu::drive {
inline mission_control::mc_commands validate_commands(
  mission_control::mc_commands p_commands)
{

  if (p_commands.mode == 'D') {
    p_commands.angle = std::clamp(p_commands.angle, -settings::max_drive_angle, settings::max_drive_angle);
  }
  // TODO: implement heartbeat logic
  if (!p_commands.is_operational) {
    p_commands.speed = 0;
    return p_commands;
  }

  p_commands.speed = std::clamp(p_commands.speed, -settings::max_speed, settings::max_speed);

  return p_commands;
}
}  // namespace sjsu::drive
