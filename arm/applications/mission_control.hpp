#pragma once

#include <libhal-util/serial.hpp>

namespace sjsu::arm{

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

  hal::status print(hal::serial& terminal)
  {
    hal::print<128>(terminal,
                    kResponseBodyFormat,
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

class mission_control {
    public:
    hal::result<mc_commands> get_commands() = 0;
    hal::status send_commands() = 0;
};

}
