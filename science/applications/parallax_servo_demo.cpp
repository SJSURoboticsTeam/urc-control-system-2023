#pragma once

#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../hardware_map.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map) {
    // science robot entry point here. This function may yield an error.
    // configure drivers
    auto& pwm = *p_map.revolver_spinner;
    auto& seal_pwm = *p_map.seal;
    auto& clock = *p_map.clock;
    auto& i2c = *p_map.i2c;
    (void)hal::write(*p_map.terminal, "moving in1");
    auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
    (void)hal::write(*p_map.terminal, "moving in1");
    HAL_CHECK(hal::delay(clock, 10ms));
    auto pca_pwm_0 = pca9685.get_pwm_channel<0>();

    HAL_CHECK(pwm.frequency(50.0_Hz));
    HAL_CHECK(pca_pwm_0.frequency(1500.0_kHz));
    // HAL_CHECK(seal_pwm.frequency(50.0_Hz));
    
    while (true) {
        (void)hal::write(*p_map.terminal, "moving in");
        HAL_CHECK(pwm.duty_cycle(0.065f));
        HAL_CHECK(hal::delay(clock, 10ms));
        HAL_CHECK(pca_pwm_0.duty_cycle(0.45f));
        HAL_CHECK(hal::delay(clock, 10ms));
        HAL_CHECK(seal_pwm.duty_cycle(0.025f));
        HAL_CHECK(hal::delay(clock, 5s));

        HAL_CHECK(pwm.duty_cycle(0.075f));
        HAL_CHECK(hal::delay(clock, 10ms));
        HAL_CHECK(pca_pwm_0.duty_cycle(0.0f));
        HAL_CHECK(hal::delay(clock, 10ms));
        HAL_CHECK(seal_pwm.duty_cycle(0.075f));
        HAL_CHECK(hal::delay(clock, 5s));

        HAL_CHECK(pwm.duty_cycle(0.085f));
        HAL_CHECK(hal::delay(clock, 10ms));
        HAL_CHECK(pca_pwm_0.duty_cycle(0.45f));
        HAL_CHECK(hal::delay(clock, 10ms));
        HAL_CHECK(seal_pwm.duty_cycle(0.125f));
        HAL_CHECK(hal::delay(clock, 5s));
    }

    return hal::success();
}