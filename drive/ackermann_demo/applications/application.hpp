#pragma once
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/motor.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>
#include <libhal/input_pin.hpp>

#include "../platform-implementations/ackermann_steering.hpp"
namespace sjsu::drive {

// for four legs, the order of legs in the span need's to be front-left, front-right, back-left, back-right
// for three legs, the order of legs in the span need's to be left, right, back
struct application_framework
{
  ackermann_steering* steering;

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
