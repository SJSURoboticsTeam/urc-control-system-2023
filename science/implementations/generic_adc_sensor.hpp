#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

class GenericAdcSensor {
    
protected:
    hal::input_pin* digital_detector_ = nullptr;
    hal::adc* adc_data_;

public:
    GenericAdcSensor(
        hal::adc* adc_data, 
        hal::input_pin* digital_detector = nullptr): digital_detector_{digital_detector}, adc_data_{adc_data} {}

    hal::result<bool> detect_signal() {
        if (digital_detector_ == nullptr) {
            return hal::new_error("Undefined digital pin");
        }
        bool result = HAL_CHECK(digital_detector_->level());
        return result;
    }


    // TODO: comment why 10
    hal::result<float> read_raw_adc() {
        float raw_ratio_average = 0;
        for (int i = 0; i < 10; i++)
        {
            auto result = HAL_CHECK(adc_data_->read());
        }
        
        raw_ratio_average /= 10;

        return raw_ratio_average;
    }

    virtual hal::result<float> read_convert_data() = 0;
};