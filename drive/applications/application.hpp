#pragma once
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/motor.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>
#include "../platform-implementations/speed_sensor.hpp"
#include "../platform-implementations/mission_control.hpp"

namespace sjsu::drive {

struct leg {
  hal::servo* steer;
  hal::motor* propulsion;
  speed_sensor* spd_sensor; 
};

struct application_framework
{
  leg* back_leg;
  leg* right_leg;
  leg* left_leg;

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
