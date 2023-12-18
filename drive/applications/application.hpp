#pragma once
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/motor.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>
#include <libhal/input_pin.hpp>
#include "../src/speed_sensor.hpp"
#include "../src/mission_control.hpp"
#include "../src/offset_servo.hpp"

namespace sjsu::drive {

struct leg {
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
hal::status initialize_processor();
hal::result<application_framework> initialize_platform();
hal::status application(application_framework& p_framework);

}
