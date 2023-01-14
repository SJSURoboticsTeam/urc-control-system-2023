#pragma once

#include <libhal/can.hpp>
#include <libhal/functional.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

namespace drive {
struct hardware_map
{
  hal::serial* terminal;
  hal::can* can;
  hal::input_pin* in_pin0;
  hal::input_pin* in_pin1;
  hal::input_pin* in_pin2;
  hal::serial* esp;
  hal::steady_clock* steady_clock;
  hal::callback<void()> reset;
};

}  // namespace drive

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status application(drive::hardware_map& p_map);
hal::result<drive::hardware_map> initialize_target();
