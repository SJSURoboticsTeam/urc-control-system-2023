#pragma once

#include <cstdint>
#include <string_view>

namespace Arm {
// joint_angles order [rotunda, shoulder, elbow, wrist_pitch, wrist_yaw]
// hand_angles order [pinky, ring, middle, index, thumb]
const char kResponseBodyFormat[] =
  "{\"heartbeat_count\":%d,\"is_operational\":%d,\"speed\":%d,\"angles\":[%d,%d,%d,%d,%d,%d]}\n";
const char kGETRequestFormat[] =
  "arm?heartbeat_count=%d&is_operational=%d&speed=%d&rotunda_angle=%d&shoulder_"
  "angle=%d&elbow_angle=%d&wrist_pitch_angle=%d&wrist_roll_angle=%d&rr9_angle=%d";

struct rr9_arguments
{
  int angle = 0;
};

struct mc_commands
{
  int heartbeat_count = 0;
  int is_operational = 0;
  int speed = 1;
  int rotunda_angle = 0;
  int shoulder_angle = 0;
  int elbow_angle = 0;
  int wrist_pitch_angle = 0;
  int wrist_roll_angle = 0;
  int rr9_angle = 0;
  hal::status Print(hal::serial &terminal)
  {
    hal::print<128>(terminal, "%d, %d, %d, %d, %d, %d, %d, %d, %d", 
           heartbeat_count,
           is_operational,
           speed,
           rotunda_angle,
           shoulder_angle,
           elbow_angle,
           wrist_pitch_angle,
           wrist_roll_angle,
           rr9_angle);
      return hal::success();
  }
};
}  // namespace Arm