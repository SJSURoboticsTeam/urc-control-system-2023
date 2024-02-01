#pragma once
#include <libhal/adc.hpp>
#include <libhal/can.hpp>
#include <libhal/functional.hpp>
#include <libhal/i2c.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/pwm.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

#include "../include/pwm_relay.hpp"

namespace sjsu::drive {

struct application_framework
{
  hal::serial* terminal;
  hal::can* can;
  hal::input_pin* in_pin0;
  hal::input_pin* in_pin1;
  hal::input_pin* in_pin2;
  hal::pwm* pwm_1_5;
  hal::adc* adc_4;
  hal::adc* adc_5;
  hal::serial* esp;
  hal::i2c* i2c;
  hal::steady_clock* steady_clock;
  hal::callback<void()> reset;
  relay* motor_relay;
};

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status initialize_processor();
hal::result<application_framework> initialize_platform();
hal::status application(application_framework& p_framework);

}
