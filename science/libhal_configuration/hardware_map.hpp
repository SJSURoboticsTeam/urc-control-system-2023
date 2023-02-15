#pragma once
#include <libhal/functional.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/serial.hpp>
#include <libhal/i2c.hpp>
namespace science {

struct hardware_map {
    hal::input_pin* is_methane;
    hal::adc* methane_level;
    hal::steady_clock* clock;
    hal::serial* science_serial;
    hal::i2c* co2i2c; 
    hal::callback<void()> reset; // reset function
};
} // namespace science

hal::result<science::hardware_map> initialize_target();