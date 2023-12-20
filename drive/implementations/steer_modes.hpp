#pragma once
#include <algorithm>
#include <cmath>

#include "../include/mission_control.hpp"
#include "../dto/drive.hpp"

namespace sjsu::drive {

static constexpr float left_leg_drive_offset = 37;   // 41
static constexpr float right_leg_drive_offset = 199.5; // 200
static constexpr float back_leg_drive_offset = 117;  // 122

static constexpr float left_leg_spin_offset = 242;
static constexpr float right_leg_spin_offset = 0;
static constexpr float back_leg_spin_offset = 0;


inline tri_wheel_router_arguments spin_steering(mission_control::mc_commands p_commands)
{
  tri_wheel_router_arguments temp;
  temp.back.angle = back_leg_spin_offset;
  temp.left.angle = left_leg_spin_offset;
  temp.right.angle = right_leg_spin_offset;
  temp.back.speed = static_cast<float>(-p_commands.speed);
  temp.left.speed = static_cast<float>(p_commands.speed);
  temp.right.speed = static_cast<float>(-p_commands.speed);

  return temp;
}

inline tri_wheel_router_arguments translate_steering(mission_control::mc_commands p_commands)
{
  tri_wheel_router_arguments steer_arguments;

  steer_arguments.left.angle =
    (static_cast<float>(-p_commands.angle) + left_leg_drive_offset);
  steer_arguments.right.angle =
    (static_cast<float>(-p_commands.angle) + right_leg_drive_offset);
  steer_arguments.back.angle =
    (static_cast<float>(-p_commands.angle) + back_leg_drive_offset);

  steer_arguments.left.speed = static_cast<float>(-p_commands.speed);
  steer_arguments.right.speed = static_cast<float>(-p_commands.speed);
  steer_arguments.back.speed = static_cast<float>(p_commands.speed);

  return steer_arguments;
}

/// Ackerman steering equation to compute outter wheel angle
inline float calculate_ackerman(float p_outter_wheel_angle)
{
  float inner_wheel_angle = float(-.583 + 1.97 * abs(int(p_outter_wheel_angle)) +
                                  -.224 * pow(abs(int(p_outter_wheel_angle)), 2) +
                                  .0278 * pow(abs(int(p_outter_wheel_angle)), 3));
  return (p_outter_wheel_angle > 0) ? inner_wheel_angle : -inner_wheel_angle;
}

inline float get_inner_wheel_radius(float p_outter_wheel_angle)
{
  return (15.0f * std::pow(std::abs(p_outter_wheel_angle), -.971f));
}

inline float get_back_wheel_radius(float p_outter_wheel_angle)
{
  return (11.6f * std::pow(std::abs(p_outter_wheel_angle), -.698f));
}

inline float get_outter_wheel_radius(float p_outter_wheel_angle)
{
  return (11.6f * std::pow(std::abs(p_outter_wheel_angle), -.616f));
}

inline float get_inner_wheel_hub_speed(float p_outter_wheel_speed,
                                       float p_outter_wheel_angle)
{
  // clamps the inner wheel speed to be no faster then what will mess up the
  // correct ackerman velocity this clamp will then ensure the same for the
  // back wheel speed since its based on this angle
  float ratio = get_inner_wheel_radius(p_outter_wheel_angle) /
                get_outter_wheel_radius(p_outter_wheel_angle);
  // std::clamp(inner_wheel_speed, -100 / ratio, 100 / ratio);
  return (p_outter_wheel_speed * ratio);
}

inline float get_back_wheel_hub_speed(float p_outter_wheel_speed,
                                      float p_outter_wheel_angle)
{
  float ratio = get_back_wheel_radius(p_outter_wheel_angle) /
                get_outter_wheel_radius(p_outter_wheel_angle);
  // std::clamp(inner_wheel_speed * ratio, -100 / ratio, 100 / ratio);
  return (p_outter_wheel_speed * ratio);
}

inline tri_wheel_router_arguments drive_steering(mission_control::mc_commands p_commands)
{
  float outter_wheel_angle = 0.0, back_wheel_angle = 0.0;
  tri_wheel_router_arguments steer_arguments;

  if (p_commands.angle > 0) {
    outter_wheel_angle = static_cast<float>(-p_commands.angle);
    steer_arguments.left.angle = outter_wheel_angle;
    steer_arguments.right.angle = calculate_ackerman(outter_wheel_angle);
  } else if (p_commands.angle < 0) {
    outter_wheel_angle = static_cast<float>(-p_commands.angle);
    steer_arguments.right.angle = outter_wheel_angle;
    steer_arguments.left.angle = calculate_ackerman(outter_wheel_angle);
  }
  if (p_commands.angle == 0) {
    steer_arguments.back.angle = 0;
    steer_arguments.left.angle = 0;
    steer_arguments.right.angle = 0;
  } else {
    outter_wheel_angle = -outter_wheel_angle;
    back_wheel_angle = float(-.0474 + -1.93 * abs(static_cast<int>(outter_wheel_angle)) +
                             -.0813 * pow(abs(static_cast<int>(outter_wheel_angle)), 2) +
                             .000555 * pow(abs(static_cast<int>(outter_wheel_angle)), 3));
    back_wheel_angle =
      (outter_wheel_angle > 0) ? -back_wheel_angle : back_wheel_angle;
    steer_arguments.back.angle = back_wheel_angle;
  }

  if (outter_wheel_angle > 0) {
    steer_arguments.right.speed = -get_inner_wheel_hub_speed(
      static_cast<float>(p_commands.speed), outter_wheel_angle);
    steer_arguments.left.speed = -static_cast<float>(p_commands.speed);
    steer_arguments.back.speed = get_back_wheel_hub_speed(
      static_cast<float>(p_commands.speed), outter_wheel_angle);
  }

  else if (outter_wheel_angle < 0) {
    steer_arguments.left.speed = -get_inner_wheel_hub_speed(
      static_cast<float>(p_commands.speed), outter_wheel_angle);
    steer_arguments.right.speed = -static_cast<float>(p_commands.speed);
    steer_arguments.back.speed = get_back_wheel_hub_speed(
      static_cast<float>(p_commands.speed), outter_wheel_angle);
  } else {
    steer_arguments.left.speed = static_cast<float>(-p_commands.speed);
    steer_arguments.right.speed = static_cast<float>(-p_commands.speed);
    steer_arguments.back.speed = static_cast<float>(p_commands.speed);
  }

  // adding the offsets to put it into the base drive mode
  steer_arguments.left.angle += left_leg_drive_offset;
  steer_arguments.right.angle += right_leg_drive_offset;
  steer_arguments.back.angle += back_leg_drive_offset;
  return steer_arguments;
}

}  // namespace Drive
