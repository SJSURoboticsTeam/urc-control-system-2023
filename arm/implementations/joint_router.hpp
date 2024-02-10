#pragma once
#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>
#include <libhal/accelerometer.hpp>

#include "../platform-implementations/mission_control.hpp"

static constexpr float shoulder_gear_ratio = 65.0f/30.0f;
static constexpr float elbow_gear_ratio = 40.0f/30.0f;

namespace sjsu::arm {
class joint_router
{
public:
  joint_router(hal::servo& p_rotunda_servo,
               hal::servo& p_shoulder_servo,
               hal::servo& p_elbow_servo,
               hal::servo& p_left_wrist_servo,
               hal::servo& p_right_wrist_servo,
               hal::servo& p_end_effector,
               hal::accelerometer& p_rotunda_accelerometer,
               hal::accelerometer& p_shoulder_accelerometer,
               hal::accelerometer& p_elbow_accelerometer,
               hal::accelerometer& p_wrist_accelerometer)
    : m_rotunda_servo(p_rotunda_servo)
    , m_shoulder_servo(p_shoulder_servo)
    , m_elbow_servo(p_elbow_servo)
    , m_left_wrist_servo(p_left_wrist_servo)
    , m_right_wrist_servo(p_right_wrist_servo)
    , m_end_effector(p_end_effector)
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

    int left_wrist_angle = (p_arguments.wrist_roll_angle) + (p_arguments.wrist_pitch_angle);
    HAL_CHECK(m_left_wrist_servo.position(static_cast<float>(left_wrist_angle)));

    int right_wrist_angle = (p_arguments.wrist_roll_angle) - (p_arguments.wrist_pitch_angle);
    HAL_CHECK(m_right_wrist_servo.position(static_cast<float>((right_wrist_angle))));
    
    float location = (std::clamp(static_cast<float>(p_arguments.rr9_angle),10.0f,100.0f)/100) * 120; //clamps to value
    // hal::print<200>(terminal, "Go to: %f\n", location);
    HAL_CHECK(m_end_effector.position(location));
    return p_arguments;
  }

private:
  hal::servo& m_rotunda_servo;
  hal::servo& m_shoulder_servo;
  hal::servo& m_elbow_servo;
  hal::servo& m_left_wrist_servo;
  hal::servo& m_right_wrist_servo;
  hal::servo& m_end_effector;

  hal::accelerometer& m_rotunda_accelerometer;
  hal::accelerometer& m_shoulder_accelerometer;
  hal::accelerometer& m_elbow_accelerometer;
  hal::accelerometer& m_wrist_accelerometer;
};
}  // namespace sjsu::arm