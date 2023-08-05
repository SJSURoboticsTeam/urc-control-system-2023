#pragma once

#include <libhal-util/serial.hpp>

namespace sjsu::arm{

  const char kResponseBodyFormat[] =
    "{\"heartbeat_count\":%d,\"is_operational\":%d,\"speed\":%d,\"angles\":[%d,%"
    "d,%d,%d,%d,%d]}\n";
  const char kGETRequestFormat[] =
    "arm?heartbeat_count=%d&is_operational=%d&speed=%d&rotunda_angle=%d&shoulder_"
    "angle=%d&elbow_angle=%d&wrist_pitch_angle=%d&wrist_roll_angle=%d&end_effector_angle=%"
    "d";

class mission_control {
  public:

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
  };


  virtual hal::result<mc_commands> get_commands() = 0;
  virtual hal::status send_commands() = 0;
};

}
