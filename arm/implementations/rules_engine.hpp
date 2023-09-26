#pragma once

#include "../platform-implementations/mission_control.hpp"
namespace sjsu::arm {
// static constexpr int kMaxSpeed = 20;
// static constexpr int kMinShoulderAngle = 0;
// static constexpr int kMaxShoulderAngle = 90;
// static constexpr int kMinElbowAngle = -150;
// static constexpr int kMaxElbowAngle = 150;
// static constexpr int kMinWristPitchAngle = -90;
// static constexpr int kMaxWristPitchAngle = 90;

inline mission_control::mc_commands validate_commands(mission_control::mc_commands commands)
{

  if (commands.speed < 1 || commands.speed > 5) {
    commands.speed = 1;
  }

  if (!commands.is_operational) {
    commands.speed = 1;
  }
  return commands;
}
}  // namespace arm