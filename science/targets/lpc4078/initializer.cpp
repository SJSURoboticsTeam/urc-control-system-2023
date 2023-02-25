#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-lpc40xx/uart.hpp>
#include <libhal-lpc40xx/adc.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-lpc40xx/i2c.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../../hardware_map.hpp"

// TODO: update with proper hardware data
constexpr int METHANE_DIGITAL_PORT = 0;
constexpr int METHANE_DIGITAL_PIN = 0;
constexpr int METHANE_ANALOG_CHANNEL = 4;

constexpr int PRESSURE_SENSOR_ANALOG = 5;

constexpr int CAN_BUS = 2;

constexpr int I2C_CHANNEL = 2;

//halleffect sensor port and pin
constexpr int HALL_EFFECT_DIGITAL_PORT = 1;
constexpr int HALL_EFFECT_DIGITAL_PIN = 15;

hal::result<science::hardware_map> initialize_target() {
    using namespace std::chrono_literals;
    using namespace hal::literals;
    hal::cortex_m::initialize_data_section();
    hal::cortex_m::system_control::initialize_floating_point_unit();
    HAL_CHECK(hal::lpc40xx::clock::maximum(12.0_MHz));
    // Create a hardware counter
    auto& clock = hal::lpc40xx::clock::get();
    auto cpu_frequency = clock.get_frequency(hal::lpc40xx::peripheral::cpu);
    static hal::cortex_m::dwt_counter counter(cpu_frequency);
    
    // create output data pins
    auto& methane_level = HAL_CHECK(hal::lpc40xx::adc::get<METHANE_ANALOG_CHANNEL>());
    auto& pressure_sensor_pin = HAL_CHECK(hal::lpc40xx::adc::get<PRESSURE_SENSOR_ANALOG>());

    auto& halleffect = HAL_CHECK((hal::lpc40xx::input_pin::get<HALL_EFFECT_DIGITAL_PORT, HALL_EFFECT_DIGITAL_PIN>()));
    hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
    auto& can = HAL_CHECK((hal::lpc40xx::can::get<CAN_BUS>(can_settings)));
    auto& pwm = HAL_CHECK((hal::lpc40xx::pwm::get<1,1>()));
    auto& i2c = HAL_CHECK((hal::lpc40xx::i2c::get<2>(hal::i2c::settings{
    .clock_rate = 100.0_kHz,})));
    // Get and initialize UART0 for UART based terminal logging
    auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
        .baud_rate = 38400,
    })));

    // Use i2c bus 2 for the dev 2 board while testing 
    auto& carbon_dioxide_sensor = HAL_CHECK((hal::lpc40xx::i2c::get<I2C_CHANNEL>()));
    // auto& i2c = HAL_CHECK((hal::lpc40xx::i2c::get<0>()));

    return science::hardware_map {
        .methane_level = &methane_level,
        .revolver_hall_effect = &halleffect,
        .seal_hall_effect = &halleffect,
        .clock = &counter,
        .terminal = &uart0,
        .reset = []() { hal::cortex_m::system_control::reset(); },
        .revolver_spinner = &pwm,
        .pressure_sensor_pin = &pressure_sensor_pin,
        .i2c = &i2c,
        .can = &can,
        
    };
}