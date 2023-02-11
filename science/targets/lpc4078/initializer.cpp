#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-lpc40xx/uart.hpp>
#include <libhal-lpc40xx/adc.hpp>

#include "hardware_map.hpp"
// TODO: update with proper hardware data
constexpr int METHANE_DIGITAL_PORT = 0;
constexpr int METHANE_DIGITAL_PIN = 0;
constexpr int METHANE_ANALOG_CHANNEL = 4;

constexpr int PRESSURE_SENSOR_ANALOG = 5;

hal::result<science::hardware_map> initialize_target() {
    hal::cortex_m::initialize_data_section();
    hal::cortex_m::system_control::initialize_floating_point_unit();

    // Create a hardware counter
    auto& clock = hal::lpc40xx::clock::get();
    auto cpu_frequency = clock.get_frequency(hal::lpc40xx::peripheral::cpu);
    static hal::cortex_m::dwt_counter counter(cpu_frequency);
    
    // create output data pins
    auto& is_methane = HAL_CHECK((hal::lpc40xx::input_pin::get<METHANE_DIGITAL_PORT, METHANE_DIGITAL_PIN>()));
    auto& methane_level = HAL_CHECK(hal::lpc40xx::adc::get<METHANE_ANALOG_CHANNEL>());
    auto& pressure_sensor_pin = HAL_CHECK(hal::lpc40xx::adc::get<PRESSURE_SENSOR_ANALOG>());

    // Get and initialize UART0 for UART based terminal logging
    auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
        .baud_rate = 38400,
    })));

    

    return science::hardware_map {
        .is_methane = &is_methane,
        .methane_level = &methane_level,
        .clock = &counter,
        .science_serial = &uart0,
        .reset = []() { hal::cortex_m::system_control::reset(); },
        .pressure_sensor_pin = &pressure_sensor_pin,
    };
}