#pragma once

#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "../hardware_map.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map) {
    // science robot entry point here. This function may yield an error.
    // configure drivers
    auto& pwm = *p_map.revolver_spinner;
    auto& clock = *p_map.clock;
    HAL_CHECK(pwm.frequency(50.0_Hz));
    while (true) {
        HAL_CHECK(pwm.duty_cycle(0.065f));
        HAL_CHECK(hal::delay(clock, 2s));
        HAL_CHECK(pwm.duty_cycle(0.075f));
        HAL_CHECK(hal::delay(clock, 2s));
        HAL_CHECK(pwm.duty_cycle(0.085f));
        HAL_CHECK(hal::delay(clock, 2s));
    }

    return hal::success();
}