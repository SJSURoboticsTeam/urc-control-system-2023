#pragma once
#include <libhal/functional.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/serial.hpp>
#include <libhal/pwm.hpp>
#include <libhal/i2c.hpp>
#include <libhal/can.hpp>

namespace science {

struct hardware_map {
    hal::input_pin* is_methane;
    hal::adc* methane_level;
    hal::input_pin* revolver_hall_effect;
    hal::input_pin* seal_hall_effect;
    hal::steady_clock* clock;
    hal::serial* terminal;
    hal::callback<void()> reset;
    hal::pwm* revolver_spinner;
    
    hal::adc* pressure_sensor_pin;
    hal::i2c* i2c;
    hal::can* can;
    hal::pwm* seal_servo;
};
} // namespace science

hal::result<science::hardware_map> initialize_target();
hal::status application(science::hardware_map& p_map);