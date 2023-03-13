#pragma once
#include <libhal/adc.hpp>
#include <libhal/can.hpp>
#include <libhal/functional.hpp>
#include <libhal/i2c.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/pwm.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

namespace science {

struct hardware_map
{
  hal::serial* terminal;
  hal::can* can;
  hal::input_pin* in_pin0;
  hal::input_pin* in_pin1;
  hal::input_pin* in_pin2;
  hal::pwm* pwm_1_6;
  hal::pwm* pwm_1_5;
  hal::adc* adc_4;
  hal::adc* adc_5;
  hal::i2c* i2c;
  hal::steady_clock* steady_clock;
  hal::callback<void()> reset;
};
}  // namespace science

hal::result<science::hardware_map> initialize_target();
hal::status application(science::hardware_map& p_map);