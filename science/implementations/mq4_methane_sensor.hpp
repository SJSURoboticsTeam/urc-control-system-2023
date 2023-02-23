#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

#include "generic_adc_sensor.hpp"

namespace science{

/// @brief Implementation driver for MQ4 methane sensor
class Mq4MethaneSensor {
public:

    /// @brief Builds MQ4 sensor driver object.
    /// @param adc_data libhal adc pin that has been initialized 
    /// @param digital_detector Digital pin that goes HIGH when methane is detected. 
    Mq4MethaneSensor(hal::adc& adc_data, hal::input_pin& digital_detector) : adc_data_(adc_data), digital_detector_(digital_detector) {};

    hal::result<float> read_raw_adc() {
        float raw_ratio_average = 0;
        int read_count = 10;
        for (int i = 0; i < read_count; i++)
        {
            raw_ratio_average += HAL_CHECK(adc_data_.read()).sample;
        }
        
        raw_ratio_average /= read_count;

        return raw_ratio_average;
    }

    /// @brief Returns read value from sensor in parts per million (PPM).
    /// @return The amount of methane near the sensor in PPM.
    hal::result<float> get_parsed_data() {
        float raw_adc_value = HAL_CHECK(read_raw_adc());

        float ppm_value = static_cast<float>(std::pow(1000 * raw_adc_value, -2.95));
        return ppm_value;
    }
    private:
    hal::adc& adc_data_; 
    hal::input_pin& digital_detector_;
};
}