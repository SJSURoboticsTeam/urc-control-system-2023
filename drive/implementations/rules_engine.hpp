#pragma once

#include "../include/mission_control.hpp"

namespace sjsu::drive {
inline mission_control::mc_commands validate_commands(
  mission_control::mc_commands p_commands)
{
  static constexpr int kMaxSpeed = 100;
  static constexpr int kMaxAngle = 12;

  if (p_commands.mode == 'D') {
    p_commands.angle = std::clamp(p_commands.angle, -kMaxAngle, kMaxAngle);
  }
  // TODO: implement heartbeat logic
  if (!p_commands.is_operational) {
    p_commands.speed = 0;
    return p_commands;
  }
  if (p_commands.speed > kMaxSpeed || p_commands.speed < -kMaxSpeed) {
    p_commands.speed = std::clamp(p_commands.speed, -kMaxSpeed, kMaxSpeed);
  }

  return p_commands;
}
}  // namespace sjsu::drive
