#pragma once
#include <libhal/functional.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/serial.hpp>
#include <libhal/pwm.hpp>
#include <libhal/i2c.hpp>

namespace science {

struct hardware_map {
    hal::input_pin* is_methane;
    hal::adc* methane_level;
    hal::input_pin* halleffect;
    hal::steady_clock* clock;
    hal::serial* science_serial;
    hal::callback<void()> reset;
    hal::pwm* air_pump;
    hal::pwm* dosing_pump;
    hal::adc* pressure_sensor_pin;
    hal::i2c* carbon_dioxide_sensor;
    hal::can* can;
};
} // namespace science

hal::result<science::hardware_map> initialize_target();
hal::status application(science::hardware_map& p_map);