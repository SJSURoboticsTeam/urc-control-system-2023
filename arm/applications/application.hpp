#pragma once
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>
#include <libhal/accelerometer.hpp>

#include "../platform-implementations/mission_control.hpp"
#include "../platform-implementations/offset_servo.hpp"

namespace sjsu::arm {
struct application_framework
{
  hal::servo* rotunda_servo;
  hal::servo* shoulder_servo;
  hal::servo* elbow_servo;
  hal::servo* left_wrist_servo;
  hal::servo* right_wrist_servo;

  hal::accelerometer* rotunda_accelerometer;
  hal::accelerometer* shoulder_accelerometer;
  hal::accelerometer* elbow_accelerometer;
  hal::accelerometer* wrist_accelerometer;
  
  // hal::servo* end_effector;
  hal::serial* terminal;
  mission_control* mc;
  hal::steady_clock* clock;
  hal::callback<void()> reset;
};

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status initialize_processor();
hal::result<application_framework> initialize_platform();
hal::status application(application_framework& p_framework);

}  // namespace sjsu::arm
