#pragma once

#include "../dto/drive.hpp"
#include "../dto/motor_feedback.hpp"
#include "../include/mission_control.hpp"

namespace sjsu::drive {
class mode_switch
{
public:
  // Should handle all the logic for switching to new steer mode
  mission_control::mc_commands switch_steer_mode(
    mission_control::mc_commands p_commands,
    tri_wheel_router_arguments p_previous_arguments,
    motor_feedback p_current_motor_feedback);

private:

  char m_previous_mode =
    'H';  // This is H for homing when the rover turns on, it makes sure that we
          // don't switch to drive mode and allow commands to be parsed through
  bool m_in_the_middle_of_switching_modes = true;
  bool m_skip_once = true;

};

}  // namespace Drive
