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
        HAL_CHECK(pwm.duty_cycle(0.061f));
        HAL_CHECK(hal::delay(clock, 5s));
        HAL_CHECK(pwm.duty_cycle(0.0697f));
        HAL_CHECK(hal::delay(clock, 5s));
        HAL_CHECK(pwm.duty_cycle(0.078f));
        HAL_CHECK(hal::delay(clock, 5s));
    }

    return hal::success();
}