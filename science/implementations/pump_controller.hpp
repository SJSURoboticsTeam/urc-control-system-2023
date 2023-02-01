#pragma once

#include <libhal/pwm.hpp>
#include <libhal/steady_clock.hpp>
#include "pressure_sensor_driver.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

const float DUTY_CYCLE = 10; // TODO: get info from electrical.

class PumpPwmController {
private:
    hal::pwm* data_pin_;
    hal::hertz current_frequency_;
    PressureSensor& pressure_sensor_;
    
    

    const int MAX_VALUE = 1000; // TODO: update with real value.

    hal::status driver_set_frequncy(hal::hertz frequency) {
        HAL_CHECK(data_pin_->frequency(frequency));
        current_frequency_ = frequency;
        return hal::success();
    }

    hal::status set_frequncy(hal::hertz frequency) {
        return driver_set_frequncy(frequency);
    }

    hal::status set_frequncy(float frequency) {
        hal::hertz frequency_hz = static_cast<hal::hertz>(frequency);
        return driver_set_frequncy(frequency_hz);
    }

    hal::status set_duty_cycle() {
        data_pin_->duty_cycle(DUTY_CYCLE);
    }

public:
    PumpPwmController(hal::pwm* data_pin, hal::hertz frequency, PressureSensor& pressure_sensor): 
    data_pin_{data_pin}, 
    current_frequency_{frequency}, pressure_sensor_{pressure_sensor} {
        set_duty_cycle();
    };
    
   

    hal::hertz frequency() {
        return current_frequency_;
    }

    hal::status start_flow(float rate) {
        // TODO: checks to make sure a real pwm value is passed
        if (rate < 0) {
            return hal::new_error("Invalid rate passed");
        }

        HAL_CHECK(set_frequncy(rate));
        return hal::success();
    }

    hal::status stop_flow() {
        HAL_CHECK(set_frequncy(0));
        return hal::success();
    }

    hal::status release_all() {
        const auto time_to_empty = 1000ms;
        HAL_CHECK(start_flow(MAX_VALUE));
        // TODO: make better
        while (!pressure_sensor_.is_pressurized())
        {
            continue;
        }
        
        HAL_CHECK(stop_flow());
    }
    
};