#pragma once

#include <cstdint>
#include <string_view>

namespace sjsu::arm {
// joint_angles order [rotunda, shoulder, elbow, wrist_pitch, wrist_yaw]
// hand_angles order [pinky, ring, middle, index, thumb]
const char kResponseBodyFormat[] =
  "{\"heartbeat_count\":%d,\"is_operational\":%d,\"speed\":%d,\"angles\":[%d,%"
  "d,%d,%d,%d,%d]}\n";
const char kGETRequestFormat[] =
  "arm?heartbeat_count=%d&is_operational=%d&speed=%d&rotunda_angle=%d&shoulder_"
  "angle=%d&elbow_angle=%d&wrist_pitch_angle=%d&wrist_roll_angle=%d&end_effector_angle=%"
  "d";

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
  int end_effector_angle = 0;
  int precision_offset = 0;
};
}  // namespace arm