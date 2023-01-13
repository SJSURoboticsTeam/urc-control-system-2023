#pragma once

#include <libhal/functional.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/can.hpp>

namespace Arm {
struct hardware_map
{
  hal::serial* terminal;
  hal::can* can;
  hal::input_pin* in_pin0;
  hal::input_pin* in_pin1;
  hal::input_pin* in_pin2;
  hal::serial* esp;
  hal::callback<void()> reset;
  hal::steady_clock* steady_clock;
};

}  // namespace Arm

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status application(starter::hardware_map& p_map);
hal::result<starter::hardware_map> initialize_target();
