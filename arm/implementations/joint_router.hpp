#pragma once
#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>

#include "../platform-implementations/mission_control.hpp"

namespace sjsu::arm {
class joint_router
{
public:
  joint_router(hal::servo& p_rotunda,
               hal::servo& p_shoulder,
               hal::servo& p_elbow,
               hal::servo& p_left_wrist,
               hal::servo& p_right_wrist)
    : m_rotunda(p_rotunda)
    , m_shoulder(p_shoulder)
    , m_elbow(p_elbow)
    , m_left_wrist(p_left_wrist)
    , m_right_wrist(p_right_wrist)
  {
  }

  hal::result<mission_control::mc_commands> move(
    mission_control::mc_commands p_arguments)
  {

    HAL_CHECK(
      m_rotunda.position(static_cast<float>(p_arguments.rotunda_angle)));

    // HAL_CHECK(
    //   m_shoulder.position(static_cast<float>(p_arguments.shoulder_angle)));

    HAL_CHECK(m_elbow.position(static_cast<float>(p_arguments.elbow_angle)));

    int left_wrist_angle = static_cast<float>(p_arguments.wrist_roll_angle) +
                           static_cast<float>(p_arguments.wrist_pitch_angle);
    HAL_CHECK(m_left_wrist.position(static_cast<float>(left_wrist_angle)));

    int right_wrist_angle = static_cast<float>(p_arguments.wrist_roll_angle) -
                            static_cast<float>(p_arguments.wrist_pitch_angle);
    HAL_CHECK(m_right_wrist.position(static_cast<float>(right_wrist_angle)));

    return p_arguments;
  }

private:
  hal::servo& m_rotunda;
  hal::servo& m_shoulder;
  hal::servo& m_elbow;
  hal::servo& m_left_wrist;
  hal::servo& m_right_wrist;
};
}  // namespace sjsu::arm