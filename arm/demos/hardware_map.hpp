#pragma once
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>
#include <libhal/pwm.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../platform-implementations/mission_control.hpp"

namespace sjsu::arm {
struct application_framework
{

  hal::servo* end_effector;
  // hal::pca::pca9685* pca;
  hal::serial* terminal;
  // mission_control* mc;
  hal::steady_clock* clock;
  hal::callback<void()> reset;
};

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status initialize_processor();
hal::result<application_framework> initialize_platform();
hal::status application(application_framework& p_framework);

}
