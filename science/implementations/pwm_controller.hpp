#pragma once

#include <libhal/pwm.hpp>

class PwmController {
private:
    hal::pwm* data_pin_;
    hal::hertz current_frequency_;

    hal::status driver_set_frequncy(hal::hertz frequency) {
        HAL_CHECK(data_pin_->frequency(frequency));
        current_frequency_ = frequency;
        return hal::success();
    }

public:
    PwmController(hal::pwm* data_pin, hal::hertz frequency): data_pin_{data_pin}, current_frequency_{frequency} {};

    hal::status set_frequncy(hal::hertz frequency) {
        return driver_set_frequncy(frequency);
    }

    hal::status set_frequncy(float frequency) {
        hal::hertz frequency_hz = static_cast<hal::hertz>(frequency);
        return driver_set_frequncy(frequency_hz);
    }

    hal::hertz frequency() {
        return current_frequency_;
    }

};