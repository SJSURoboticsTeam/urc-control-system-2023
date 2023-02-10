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
    // Methane Sensor
    hal::input_pin* is_methane;
    hal::adc* methane_level;
    
    // Various useful features to have on lpc4078
    hal::steady_clock* clock;
    hal::serial* science_serial;
    hal::callback<void()> reset; // reset function

    // Pump interfaces
    hal::pwm* air_pump;
    hal::pwm* dosing_pump;
    
    // Pressure sensor for vacuume pump
    hal::adc* pressure_sensor_pin;

    // CO2 sensor interface
    hal::i2c* carbon_dioxide_sensor;
};
} // namespace science

hal::result<science::hardware_map> initialize_target();
