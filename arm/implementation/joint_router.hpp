#pragma once
#include <libhal-rmd/drc.hpp>
#include <libhal-util/map.hpp>
#include <libhal-util/units.hpp>

#include "../../dto/arm_dto.hpp"
// #include "../soft-drivers/rmd-encoder.hpp"

namespace arm {
class joint_router
{
public:
  joint_router(hal::rmd::drc& rotunda,
               hal::rmd::drc& shoulder,
               hal::rmd::drc& elbow,
               hal::rmd::drc& left_wrist,
               hal::rmd::drc& right_wrist,
               hal::pwm& pwm0)
    : rotunda(rotunda)
    , shoulder(shoulder)
    , elbow(elbow)
    , left_wrist(left_wrist)
    , right_wrist(right_wrist)
    , pwm0(pwm0)
  {
  }

  mc_commands move(mc_commands arguments)
  {
    rotunda.position_control(
      hal::degrees(static_cast<float>(arguments.rotunda_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    shoulder.position_control(
      -hal::degrees(static_cast<float>(arguments.shoulder_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    elbow.position_control(
      hal::degrees(static_cast<float>(arguments.elbow_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    left_wrist.position_control(
      hal::degrees(static_cast<float>(arguments.wrist_pitch_angle) +
                   static_cast<float>(arguments.wrist_roll_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    right_wrist.position_control(
      hal::degrees(static_cast<float>(arguments.wrist_roll_angle) -
                   static_cast<float>(arguments.wrist_pitch_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    pwm0.duty_cycle(ConvertAngleToDutyCycle(180 - arguments.rr9_angle));
    return arguments;
  }

  void HomeArm()
  {
  }

  float ConvertAngleToDutyCycle(int angle)
  {
    std::pair<float, float> from;
    std::pair<float, float> to;
    from.first = 0.0f;
    from.second = 180.0f;
    to.first = 0.025f;
    to.second = 0.125f;
    return hal::map(static_cast<float>(angle), from, to);
  }

private:
  float initial_rotunda_position = 0;
  float initial_shoulder_position = 0;
  float initial_elbow_position = 0;
  float initial_left_wrist_position = 0;
  float initial_right_wrist_position = 0;

  hal::rmd::drc& rotunda;
  hal::rmd::drc& shoulder;
  hal::rmd::drc& elbow;
  hal::rmd::drc& left_wrist;
  hal::rmd::drc& right_wrist;
  hal::pwm& pwm0;
};
}  // namespace arm