#pragma once

#include <libhal-util/units.hpp>

#include <libhal-rmd/drc.hpp>

#include "../../dto/arm-dto.hpp"

#include "../soft-driver/rmd-encoder.hpp"

namespace Arm {
class JointRouter
{
public:
  JointRouter(hal::rmd::drc& rotunda,
              hal::rmd::drc& shoulder,
              hal::rmd::drc& elbow,
              hal::rmd::drc& left_wrist,
              hal::rmd::drc& right_wrist){};

  joint_arguments SetJointArguments(joint_arguments joint_arguments)
  {
  }

  // stopped here

  JointRouter(sjsu::RmdX& rotunda,
              sjsu::RmdX& shoulder,
              sjsu::RmdX& elbow,
              sjsu::RmdX& left_wrist,
              sjsu::RmdX& right_wrist)
    : rotunda_(rotunda)
    , shoulder_(shoulder)
    , elbow_(elbow)
    , left_wrist_(left_wrist)
    , right_wrist_(right_wrist)
  {
  }

  void Initialize()
  {
    rotunda_.Initialize();
    shoulder_.Initialize();
    elbow_.Initialize();
    left_wrist_.Initialize();
    right_wrist_.Initialize();
  };

  mc_commands SetJointArguments(mc_commands arguments)
  {
    rotunda_.SetAngle(
      units::angle::degree_t(static_cast<float>(arguments.rotunda_angle)),
      units::angular_velocity::revolutions_per_minute_t(
        static_cast<float>(arguments.speed)));
    shoulder_.SetAngle(
      units::angle::degree_t(static_cast<float>(arguments.shoulder_angle)),
      units::angular_velocity::revolutions_per_minute_t(
        static_cast<float>(arguments.speed)));
    elbow_.SetAngle(
      units::angle::degree_t(static_cast<float>(arguments.elbow_angle)),
      units::angular_velocity::revolutions_per_minute_t(
        static_cast<float>(arguments.speed)));
    left_wrist_.SetAngle(
      units::angle::degree_t(static_cast<float>(arguments.wrist_pitch_angle) +
                             static_cast<float>(arguments.wrist_roll_angle)),
      units::angular_velocity::revolutions_per_minute_t(
        static_cast<float>(arguments.speed)));
    right_wrist_.SetAngle(
      units::angle::degree_t(static_cast<float>(arguments.wrist_roll_angle) -
                             static_cast<float>(arguments.wrist_pitch_angle)),
      units::angular_velocity::revolutions_per_minute_t(
        static_cast<float>(arguments.speed)));
    return arguments;
  }

  void HomeArm()
  {
    sjsu::LogInfo("Homing arm...");
    initial_rotunda_position_ =
      common::RmdEncoder::CalcEncoderPositions(rotunda_);
    initial_shoulder_position_ =
      common::RmdEncoder::CalcEncoderPositions(shoulder_);
    initial_elbow_position_ = common::RmdEncoder::CalcEncoderPositions(elbow_);
    initial_left_wrist_position_ =
      common::RmdEncoder::CalcEncoderPositions(left_wrist_);
    initial_right_wrist_position_ =
      common::RmdEncoder::CalcEncoderPositions(right_wrist_);
  }

private:
  float initial_rotunda_position_ = 0;
  float initial_shoulder_position_ = 0;
  float initial_elbow_position_ = 0;
  float initial_left_wrist_position_ = 0;
  float initial_right_wrist_position_ = 0;

  sjsu::RmdX& rotunda_;
  sjsu::RmdX& shoulder_;
  sjsu::RmdX& elbow_;
  sjsu::RmdX& left_wrist_;
  sjsu::RmdX& right_wrist_;
};
}  // namespace Arm
