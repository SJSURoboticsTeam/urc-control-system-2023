#pragma once

#include <libhal/pwm.hpp>
#include <libhal/steady_clock.hpp>
#include "pressure_sensor_driver.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

namespace science{

/// @brief A driver to control a PWM controlled pump (motor). Built for electrical's new science bot PWM board
class PumpPwmController {
private:
    hal::pwm* data_pin_;
    float duty_cycle_;
    hal::hertz current_frequency_;
    
    /// @brief Sets the underlying frequency our PWM is oscillating at.
    /// @param frequency The frequency in hertz
    /// @return void return, fails check if unable to interface with the hardware.
    hal::status set_frequncy(float frequency) {
        HAL_CHECK(data_pin_->frequency(frequency));
        current_frequency_ = frequency;
        return hal::success();
    }


    /// @brief Sets the duty cycle percentage. How much of the time the should PWM be HIGH.
    /// @param duty_cycle The duty cycle percentage in decimal form. 
    /// NOTE: The motors that this board will control has a max of 6V. The board however, can supply a max of 12V
    /// duty cycle has been capped at 50% for this purpose.
    /// @return void return, fails check if unable to interface with the hardware.
    hal::status set_duty_cycle(float duty_cycle) {
        if (duty_cycle < 0 || duty_cycle > 0.5) // electrical limitaiton
            return hal::new_error("Invalid percentage for duty cycle passed");
        HAL_CHECK(data_pin_->duty_cycle(duty_cycle));
        duty_cycle_ = duty_cycle;
        return hal::success();
    }

public:
    /// @brief A driver to control a PWM controlled pump (motor). Built for electrical's new science bot PWM board
    /// Duty cycle defaults to 0%. (Completely off)
    /// @param data_pin The PWM pin to control the pump.
    /// @param frequency The oscillating frequency for PWM communication.
    PumpPwmController(hal::pwm* data_pin, hal::hertz frequency): 
    data_pin_{data_pin}, 
    current_frequency_{frequency} {
        set_duty_cycle(0);
        set_frequncy(frequency);
    };
    
   
    /// @brief Gets current frequency.
    /// @return The current frequency in herts
    hal::hertz frequency() {
        return current_frequency_;
    }

    /// @brief Gets the current duty cycle.
    /// @return The duty cycle percentage of how much the signal is HIGH in decimal form.
    float duty_cycle() {
        return duty_cycle_;
    }

    /// @brief Starts the flow of the pump.
    /// @param rate_percentage The rate at which the pump should run.
    hal::status start_flow(float rate_percentage) {
        if (rate_percentage < 0) {
            return hal::new_error("Invalid rate_percentage passed");
        }

        HAL_CHECK(set_duty_cycle(rate_percentage));
        return hal::success();
    }

    /// @brief Stops the pump.
    hal::status stop_flow() {
        HAL_CHECK(set_duty_cycle(0));
        return hal::success();
    }

    /// @brief Designed for a vacuum pump. Draws all the air out an airtight chamber. 
    /// @param sensor A pressure sensor to check when chamber is pressurized or not
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
}