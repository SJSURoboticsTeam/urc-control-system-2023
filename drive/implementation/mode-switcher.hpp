#pragma once

#include "../dto/drive-dto.hpp"
#include "../dto/motor-feedback-dto.hpp"

namespace Drive {
class mode_switch
{
public:
  // Should handle all the logic for switching to new steer mode
  drive_commands switch_steer_mode(drive_commands commands,
                                 tri_wheel_router_arguments previous_arguments,
                                 motor_feedback current_motor_feedback,
                                 hal::serial& terminal)
  {
    // motor_speeds = HAL_CHECK(tri_wheel.GetMotorFeedback(clock));
    // current_motor_feedback.Print(terminal);
    bool hubs_stopped((previous_arguments.left.hub.speed >= -0.01f &&
                       previous_arguments.left.hub.speed <= 0.01f) &&
                      (previous_arguments.right.hub.speed >= -0.01f &&
                       previous_arguments.right.hub.speed <= 0.01f) &&
                      (previous_arguments.back.hub.speed >= -0.01f &&
                       previous_arguments.back.hub.speed <= 0.01f));
    bool steers_stopped((current_motor_feedback.left_steer_speed >= -0.01f &&
                         current_motor_feedback.left_steer_speed <= 0.01f) &&
                        (current_motor_feedback.right_steer_speed >= -0.01f &&
                         current_motor_feedback.right_steer_speed <= 0.01f) &&
                        (current_motor_feedback.back_steer_speed >= -0.01f &&
                         current_motor_feedback.back_steer_speed <= 0.01f));
    if (previous_mode_ != commands.mode) {
      in_the_middle_of_switching_modes_ = true;
      skip_once_ = true;
    }
    if (in_the_middle_of_switching_modes_) {
      commands.speed = 0;
      if (!hubs_stopped) {
        commands.mode = previous_mode_;
        return commands;
      }  // hubs must be stopped to pass here
      else if (!skip_once_ && steers_stopped) {
        in_the_middle_of_switching_modes_ = false;
        skip_once_ = true;
      }  // only once steer motors have stopped moving after hubs stopped will
         // we exit switching modes
      previous_mode_ = commands.mode;
      skip_once_ = false;
    }
    return commands;
  }

private:
  char previous_mode_ =
    'H';  // This is H for homing when the rover turns on, it makes sure that we
          // don't switch to drive mode and allow commands to be parsed through
  bool in_the_middle_of_switching_modes_ = true;
  bool skip_once_ = true;
};
}  // namespace Drive