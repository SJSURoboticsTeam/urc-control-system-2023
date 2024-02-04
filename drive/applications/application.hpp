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
#include "../include/sk9822.hpp"
namespace sjsu::drive {

struct leg
{
  hal::servo* steer;
  hal::motor* propulsion;
  speed_sensor* steer_speed_sensor;
  speed_sensor* propulsion_speed_sensor;
};
struct effect_hardware
{
  light_strip_view lights;
  sk9822* driver;
  hal::steady_clock* clock;
};
struct application_framework
{
  std::span<leg*> legs;

  mission_control* mc;

  hal::serial* terminal;
  hal::steady_clock* clock;
  effect_hardware* led_strip;
  hal::callback<void()> reset;
};   

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status initialize_processor();
hal::result<application_framework> initialize_platform();
hal::status application(application_framework& p_framework);

}  // namespace sjsu::drive
