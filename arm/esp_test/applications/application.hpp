#pragma once
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::arm {
struct application_framework
{
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
