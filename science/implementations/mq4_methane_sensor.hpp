#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

#include "generic_adc_sensor.hpp"

class Mq4MethaneSensor : public GenericAdcSensor {
public:
    Mq4MethaneSensor(hal::adc* adc_data, hal::input_pin* digital_detector): GenericAdcSensor(adc_data, digital_detector) {};

    virtual hal::result<float> read_convert_data() override {
        float raw_adc_value = HAL_CHECK(read_raw_adc());

        float ppm_value = static_cast<float>(std::pow(1000 * raw_adc_value, -2.95));
        return ppm_value;
    }
};