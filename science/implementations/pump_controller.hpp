#pragma once

#include <libhal/pwm.hpp>
#include <libhal/steady_clock.hpp>
#include "pressure_sensor_driver.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;


class PumpPwmController {
private:
    hal::pwm* data_pin_;
    float duty_cycle_;
    hal::hertz current_frequency_;
    
    

    const int MAX_VALUE = 1000; // TODO: update with real value.

    hal::status set_frequncy(float frequency) {
        HAL_CHECK(data_pin_->frequency(frequency));
        current_frequency_ = frequency;
        return hal::success();
    }


    // The expected max voltage for the pumps is 6.0V. Electrical's board has a max of 12V. Therefore
    // The duty cycle for this driver is capped at 50%
    hal::status set_duty_cycle(float duty_cycle) {
        if (duty_cycle < 0 || duty_cycle > 0.5)
            return hal::new_error("Invalid percentage for duty cycle passed");
        HAL_CHECK(data_pin_->duty_cycle(duty_cycle));
        duty_cycle_ = duty_cycle;
        return hal::success();
    }

public:
    PumpPwmController(hal::pwm* data_pin, hal::hertz frequency): 
    data_pin_{data_pin}, 
    current_frequency_{frequency} {
        set_duty_cycle(0);
        set_frequncy(frequency);
    };
    
   

    hal::hertz frequency() {
        return current_frequency_;
    }

    float duty_cycle() {
        return duty_cycle_;
    }

    hal::status start_flow(float rate_percentage) {
        if (rate_percentage < 0) {
            return hal::new_error("Invalid rate_percentage passed");
        }

        HAL_CHECK(set_duty_cycle(rate_percentage));
        return hal::success();
    }

    hal::status stop_flow() {
        HAL_CHECK(set_duty_cycle(0));
        return hal::success();
    }

    // TODO: Add way to give either negative or positive voltage (6V or 6V max)
    // Unsure if we're using a pressure sensor or not.
    hal::status run_vacuum(PressureSensor& sensor) {
        return hal::new_error("Not implemented");
        HAL_CHECK(start_flow(0.5));
        while (!sensor.is_pressurized()) {
            continue;
        }
        HAL_CHECK(stop_flow());
        return hal::success();
    }
    
};