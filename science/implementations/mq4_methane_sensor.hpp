#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

class Mq4MethaneSensor {
private:
    hal::input_pin* methane_detected_;
    hal::adc* methane_data_;

public:
    Mq4MethaneSensor(
        hal::input_pin* methane_detected,
        hal::adc* methane_data):methane_detected_{methane_detected}, methane_data_{methane_data} {}

    hal::result<bool> detect_methane() {
        bool result = HAL_CHECK(methane_detected_->level());
        return result;
    }

    hal::result<float> read_methane_level() {

        float raw_ratio_average = 0;
        for (int i = 0; i < 10; i++)
            raw_ratio_average += HAL_CHECK(methane_data_->read());
        
        raw_ratio_average /= 10;

        // using the formula derived in https://www.utmel.com/components/how-to-use-mq4-gas-sensor?id=821
        // ppm for Methane (CH4) from the ratio read in by the sensor is 1000(ratio)^(-2.95)
        float ppm_value = static_cast<float>(std::pow(1000 * raw_ratio_average, -2.95));
        return ppm_value;
    }
};


