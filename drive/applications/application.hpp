#pragma once
#include "../include/mission_control.hpp"
#include "../include/offset_servo.hpp"
#include "../include/speed_sensor.hpp"
#include <libhal/input_pin.hpp>
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::drive {

struct leg
{
  hal::servo* steer;
  hal::motor* propulsion;
  speed_sensor* steer_speed_sensor;
  speed_sensor* propulsion_speed_sensor;
};

struct application_framework
{
  std::span<leg*> legs;

  mission_control* mc;

  hal::serial* terminal;
  hal::steady_clock* clock;
  hal::callback<void()> reset;
};

// Application function must be implemented by one of the compilation units
// (.cpp) files.
application_framework initialize_platform();
void application(application_framework& p_framework);

}  // namespace sjsu::drive
