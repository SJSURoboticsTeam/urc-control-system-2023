#pragma once
#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>
#include <libhal/accelerometer.hpp>

#include "../platform-implementations/mission_control.hpp"
#include "../platform-implementations/offset_servo.hpp"

static constexpr float shoulder_gear_ratio = 65/30;
static constexpr float elbow_gear_ratio = 40/30;

namespace sjsu::arm {
class joint_router
{
public:
  joint_router(offset_servo& p_rotunda_servo,
               offset_servo& p_shoulder_servo,
               offset_servo& p_elbow_servo,
               offset_servo& p_left_wrist_servo,
               offset_servo& p_right_wrist_servo,
               hal::accelerometer& p_rotunda_accelerometer,
               hal::accelerometer& p_shoulder_accelerometer,
               hal::accelerometer& p_elbow_accelerometer,
               hal::accelerometer& p_wrist_accelerometer)
    : m_rotunda_servo(p_rotunda_servo)
    , m_shoulder_servo(p_shoulder_servo)
    , m_elbow_servo(p_elbow_servo)
    , m_left_wrist_servo(p_left_wrist_servo)
    , m_right_wrist_servo(p_right_wrist_servo)
    , m_rotunda_accelerometer(p_rotunda_accelerometer)
    , m_shoulder_accelerometer(p_shoulder_accelerometer)
    , m_elbow_accelerometer(p_elbow_accelerometer)
    , m_wrist_accelerometer(p_wrist_accelerometer)
  {
  }

  hal::result<mission_control::mc_commands> move(
    mission_control::mc_commands p_arguments)
  {

    HAL_CHECK(
      m_rotunda_servo.position(static_cast<float>(p_arguments.rotunda_angle)));

    HAL_CHECK(
      m_shoulder_servo.position(static_cast<float>(-p_arguments.shoulder_angle * shoulder_gear_ratio)));

    HAL_CHECK(m_elbow_servo.position(static_cast<float>(p_arguments.elbow_angle * elbow_gear_ratio)));

    int left_wrist_angle = static_cast<float>(p_arguments.wrist_roll_angle) +
                           static_cast<float>(p_arguments.wrist_pitch_angle);
    HAL_CHECK(m_left_wrist_servo.position(static_cast<float>(left_wrist_angle)));

    int right_wrist_angle = static_cast<float>(p_arguments.wrist_roll_angle) -
                            static_cast<float>(p_arguments.wrist_pitch_angle);
    HAL_CHECK(m_right_wrist_servo.position(static_cast<float>(right_wrist_angle)));

    return p_arguments;
  }

private:
  offset_servo& m_rotunda_servo;
  offset_servo& m_shoulder_servo;
  offset_servo& m_elbow_servo;
  offset_servo& m_left_wrist_servo;
  offset_servo& m_right_wrist_servo;

  hal::accelerometer& m_rotunda_accelerometer;
  hal::accelerometer& m_shoulder_accelerometer;
  hal::accelerometer& m_elbow_accelerometer;
  hal::accelerometer& m_wrist_accelerometer;
};
}  // namespace sjsu::arm