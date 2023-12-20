#pragma once

#include <libhal-util/serial.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::drive {
struct motor_feedback
{
  float left_steer_speed = 5.0;
  float right_steer_speed = 5.0;
  float back_steer_speed = 5.0;
  float left_drive_speed = 5.0;
  float right_drive_speed = 5.0;
  float back_drive_speed = 5.0;

  hal::status Print(hal::serial& terminal)
  {
    hal::print<50>(
      terminal,
      "\nSteers: left speed: %f right speed: %f back speed: %f Drives: Steers: "
      "left speed: %f right speed: %f back speed: %f\n",
      static_cast<float>(left_steer_speed),
      static_cast<float>(right_steer_speed),
      static_cast<float>(back_steer_speed),
      static_cast<float>(left_drive_speed),
      static_cast<float>(right_drive_speed),
      static_cast<float>(back_drive_speed));
    return hal::success();
  }
};

}  // namespace sjsu::drive