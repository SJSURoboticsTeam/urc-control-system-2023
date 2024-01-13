#pragma once

#include "../platform-implementations/mission_control.hpp"

namespace sjsu::arm {

class speed_control
{
public:
  mission_control::mc_commands lerp(
    mission_control::mc_commands p_commands_to_set)
  {

    float lerp_constant = .01f;

    // lerping rotunda
    p_commands_to_set.rotunda_angle =
      std::lerp(m_current_settings.rotunda_angle,
                p_commands_to_set.rotunda_angle,
                p_commands_to_set.speed * lerp_constant);

    // lerping shoulder
    p_commands_to_set.shoulder_angle =
      std::lerp(m_current_settings.shoulder_angle,
                p_commands_to_set.shoulder_angle,
                p_commands_to_set.speed * lerp_constant);

    // lerping elbow
    p_commands_to_set.elbow_angle =
      std::lerp(m_current_settings.elbow_angle,
                p_commands_to_set.elbow_angle,
                p_commands_to_set.speed * lerp_constant);

    // lerping wrist pitch
    p_commands_to_set.wrist_pitch_angle =
      std::lerp(m_current_settings.wrist_pitch_angle,
                p_commands_to_set.wrist_pitch_angle,
                p_commands_to_set.speed * lerp_constant);

    // lerping wrist roll
    p_commands_to_set.wrist_roll_angle =
      std::lerp(m_current_settings.wrist_roll_angle,
                p_commands_to_set.wrist_roll_angle,
                p_commands_to_set.speed * lerp_constant);

    // lerping end effector
    // p_commands_to_set.end_effector_angle =
    //         std::lerp(m_current_settings.end_effector_angle,
    //                 p_commands_to_set.end_effector_angle,
    //                 p_commands_to_set.speed * lerp_constant);

    // set this to the current value to allow for lerping to the next step
    m_current_settings = p_commands_to_set;
    return p_commands_to_set;
  }

private:
  mission_control::mc_commands m_current_settings;
};

}  // namespace sjsu::arm
