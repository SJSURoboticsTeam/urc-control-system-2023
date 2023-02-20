#pragma once
#include <libhal-rmd/drc.hpp>
#include <libhal-util/map.hpp>
#include <libhal-util/units.hpp>

#include "../../dto/arm-dto.hpp"
// #include "../soft-drivers/rmd-encoder.hpp"

namespace Arm {
class JointRouter
{
public:
  JointRouter(hal::rmd::drc& rotunda,
              hal::rmd::drc& shoulder,
              hal::rmd::drc& elbow,
              hal::rmd::drc& left_wrist,
              hal::rmd::drc& right_wrist,
              hal::pwm& pwm0)
    : rotunda_(rotunda)
    , shoulder_(shoulder)
    , elbow_(elbow)
    , left_wrist_(left_wrist)
    , right_wrist_(right_wrist)
    , pwm0_(pwm0)
  {
  }

  mc_commands SetJointArguments(mc_commands arguments)
  {
    rotunda_.position_control(
      hal::degrees(static_cast<float>(arguments.rotunda_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    shoulder_.position_control(
      hal::degrees(static_cast<float>(arguments.shoulder_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    elbow_.position_control(
      hal::degrees(static_cast<float>(arguments.elbow_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    left_wrist_.position_control(
      hal::degrees(static_cast<float>(arguments.wrist_pitch_angle) +
                   static_cast<float>(arguments.wrist_roll_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    right_wrist_.position_control(
      hal::degrees(static_cast<float>(arguments.wrist_roll_angle) -
                   static_cast<float>(arguments.wrist_pitch_angle)),
      hal::rpm(static_cast<float>(arguments.speed)));
    pwm0_.duty_cycle(ConvertAngleToDutyCycle(arguments.rr9_angle));
    return arguments;
  }

  void HomeArm()
  {
    // initial_rotunda_position_ = RmdEncoder::CalcEncoderPositions(rotunda_);
    // initial_shoulder_position_ = RmdEncoder::CalcEncoderPositions(shoulder_);
    // initial_elbow_position_ = RmdEncoder::CalcEncoderPositions(elbow_);
    // initial_left_wrist_position_ =
    //   RmdEncoder::CalcEncoderPositions(left_wrist_);
    // initial_right_wrist_position_ =
    //   RmdEncoder::CalcEncoderPositions(right_wrist_);
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
  float initial_rotunda_position_ = 0;
  float initial_shoulder_position_ = 0;
  float initial_elbow_position_ = 0;
  float initial_left_wrist_position_ = 0;
  float initial_right_wrist_position_ = 0;

  hal::rmd::drc& rotunda_;
  hal::rmd::drc& shoulder_;
  hal::rmd::drc& elbow_;
  hal::rmd::drc& left_wrist_;
  hal::rmd::drc& right_wrist_;
  hal::pwm& pwm0_;
};
}  // namespace Arm