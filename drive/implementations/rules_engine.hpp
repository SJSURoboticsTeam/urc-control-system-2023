#pragma once

#include "../include/mission_control.hpp"

namespace sjsu::drive {
inline mission_control::mc_commands validate_commands(mission_control::mc_commands commands)
{
  static constexpr int kMaxSpeed = 100;
  static constexpr int kMaxAngle = 12;

  if (commands.mode == 'D') {
    commands.angle = std::clamp(commands.angle, -kMaxAngle, kMaxAngle);
  }
  // TODO: implement heartbeat logic
  if (!commands.is_operational) {
    commands.speed = 0;
    return commands;
  }
  if (commands.speed > kMaxSpeed || commands.speed < -kMaxSpeed) {
    commands.speed = std::clamp(commands.speed, -kMaxSpeed, kMaxSpeed);
  }

  return commands;
}
}  // namespace Drive