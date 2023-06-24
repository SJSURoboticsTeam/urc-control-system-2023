#pragma once

#include <libhal/adc.hpp>
#include <libhal/can.hpp>
#include <libhal/functional.hpp>
#include <libhal/i2c.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/pwm.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

namespace sjsu {
struct hardware_map
{
  hal::serial* terminal;
  hal::can* can;
  hal::input_pin* in_pin0;
  hal::input_pin* in_pin1;
  hal::input_pin* in_pin2;
  hal::output_pin* can_en;
  hal::output_pin* motor_en;
  hal::pwm* pwm_1_6;
  hal::pwm* pwm_1_5;
  hal::adc* adc_4;
  hal::adc* adc_5;
  hal::serial* esp;
  hal::i2c* i2c;
  hal::steady_clock* steady_clock;
  hal::callback<void()> reset;
};

}  // namespace sjsu

// Application function must be implemented by one of the compilation units
// (.cpp) files.
hal::status application(sjsu::hardware_map& p_map);
hal::result<sjsu::hardware_map> initialize_target();
