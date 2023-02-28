#pragma once

#include "../dto/arm-dto.hpp"
// #include "../../common/heartbeat.hpp"

namespace arm {
class RulesEngine
{
public:
  // static constexpr int kMaxSpeed = 20;
  // static constexpr int kMinShoulderAngle = 0;
  // static constexpr int kMaxShoulderAngle = 90;
  // static constexpr int kMinElbowAngle = -150;
  // static constexpr int kMaxElbowAngle = 150;
  // static constexpr int kMinWristPitchAngle = -90;
  // static constexpr int kMaxWristPitchAngle = 90;

  mc_commands ValidateCommands(mc_commands commands)
  {

    // Must test safety of 0 RPM in position_control
    if (commands.speed < 1 || commands.speed > 5) {
      commands.speed = 1;
    }

    if (!commands.is_operational) {
      commands.speed = 1;
    }
    // heartbeat_.IncrementHeartbeatCount();
    return commands;
  }

  // private:
  //     sjsu::common::Heartbeat heartbeat_;
};
}  // namespace Arm