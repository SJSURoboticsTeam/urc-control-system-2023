#include "../include/mode_switcher.hpp"
#include "../dto/drive.hpp"
#include "../dto/motor_feedback.hpp"
#include "../include/mission_control.hpp"

namespace sjsu::drive {
mission_control::mc_commands mode_switch::switch_steer_mode(
  mission_control::mc_commands p_commands,
  tri_wheel_router_arguments p_previous_arguments,
  motor_feedback p_current_motor_feedback)
{
  bool hubs_stopped((p_previous_arguments.left.speed >= -0.01f &&
                     p_previous_arguments.left.speed <= 0.01f) &&
                    (p_previous_arguments.right.speed >= -0.01f &&
                     p_previous_arguments.right.speed <= 0.01f) &&
                    (p_previous_arguments.back.speed >= -0.01f &&
                     p_previous_arguments.back.speed <= 0.01f));

  bool steers_stopped((p_current_motor_feedback.left_steer_speed >= -0.01f &&
                       p_current_motor_feedback.left_steer_speed <= 0.01f) &&
                      (p_current_motor_feedback.right_steer_speed >= -0.01f &&
                       p_current_motor_feedback.right_steer_speed <= 0.01f) &&
                      (p_current_motor_feedback.back_steer_speed >= -0.01f &&
                       p_current_motor_feedback.back_steer_speed <= 0.01f));

  if (m_previous_mode != p_commands.mode) {
    m_in_the_middle_of_switching_modes = true;
    m_skip_once = true;
  }

  if (m_in_the_middle_of_switching_modes) {
    p_commands.speed = 0;
    if (!hubs_stopped) {
      p_commands.mode = m_previous_mode;
      return p_commands;
    }  // hubs must be stopped to pass here

    else if (!m_skip_once && steers_stopped) {
      m_in_the_middle_of_switching_modes = false;
      m_skip_once = true;
    }  // only once steer motors have stopped moving after hubs stopped will
       // we exit switching modes
    m_previous_mode = p_commands.mode;
    m_skip_once = false;
  }

  return p_commands;
}
}  // namespace sjsu::drive