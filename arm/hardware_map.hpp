#pragma once

#include <libhal/can.hpp>
#include <libhal/functional.hpp>
// #include <libhal/pwm.hpp>
// #include <libhal/i2c.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

namespace drive {
struct hardware_map
{
  hal::serial* terminal;
  hal::can* can;
  hal::serial* esp;
  // hal::i2c* i2c;
  hal::steady_clock* steady_clock;
  hal::callback<void()> reset;
  // hal::pwm* pwm0;
};
}  // namespace drive

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status application(drive::hardware_map& p_map);
hal::result<drive::hardware_map> initialize_target();
