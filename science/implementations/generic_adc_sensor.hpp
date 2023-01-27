#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

class GenericAdcSensor {
    
protected:
    hal::input_pin* digital_detector_;
    hal::adc* adc_data_;

public:
    GenericAdcSensor(
        hal::input_pin* digital_detector,
        hal::adc* adc_data):digital_detector_{digital_detector}, adc_data_{adc_data} {}

    hal::result<bool> detect_methane() {
        bool result = HAL_CHECK(digital_detector_->level());
        return result;
    }

    hal::result<float> read_raw_adc() {
        float raw_ratio_average = 0;
        for (int i = 0; i < 10; i++)
            raw_ratio_average += HAL_CHECK(adc_data_->read());
        
        raw_ratio_average /= 10;

        return raw_ratio_average;
    }

    virtual hal::result<float> read_convert_data() = 0;
};