#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

#include "generic_adc_sensor.hpp"

/// @brief Implementation driver for MQ4 methane sensor
class Mq4MethaneSensor : public GenericAdcSensor {
public:

    /// @brief Builds MQ4 sensor driver object.
    /// @param adc_data libhal adc pin that has been initialized 
    /// @param digital_detector Digital pin that goes HIGH when methane is detected. 
    Mq4MethaneSensor(hal::adc* adc_data, hal::input_pin* digital_detector): GenericAdcSensor(adc_data, digital_detector) {};


    /// @brief Returns read value from sensor in parts per million (PPM).
    /// @return The amount of methane near the sensor in PPM.
    virtual hal::result<float> read_data() override {
        float raw_adc_value = HAL_CHECK(read_raw_adc());

        float ppm_value = static_cast<float>(std::pow(1000 * raw_adc_value, -2.95));
        return ppm_value;
    }
};