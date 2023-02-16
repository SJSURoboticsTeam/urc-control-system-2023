#pragma once

#include <libhal/input_pin.hpp>
#include <libhal/adc.hpp>
#include <cmath>

namespace science{

/// @brief A Generic ADC Driver. This is designed to be a higher level interface for analog devices.
class GenericAdcSensor {
public:
    /// @brief Builds driver object
    /// @param adc_data libhal adc pin that has been initialized 
    /// @param digital_detector Optional digital pin for device that is to goes high when a signal is detected
    GenericAdcSensor(hal::adc* adc_data, hal::input_pin* digital_detector = nullptr): digital_detector_{digital_detector}, 
    adc_data_{adc_data} {}

    /// @brief Returns true when a signal is being ran from device.
    /// @return The result of the status of the device.
    hal::result<bool> detect_signal() {
        if (digital_detector_ == nullptr) {
            return hal::new_error("Undefined digital pin");
        }
        bool result = HAL_CHECK(digital_detector_->level());
        return result;
    }


    /// @brief Reads the raw voltage value from the ADC pin. 
    /// Does no unit conversion outside of converting the analog signal to a digital signal.
    /// @return The voltage value on the analog data pin.
    hal::result<float> read_raw_adc() {
        float raw_ratio_average = 0;
        int read_count = 10;
        for (int i = 0; i < read_count; i++)
        {
            raw_ratio_average += HAL_CHECK(adc_data_->read());
        }
        
        raw_ratio_average /= read_count;

        return raw_ratio_average;
    }

    /// @brief Processes data into usable units.
    /// Implementations of this method should include a mathmatical conversion formula that goes from raw voltage
    /// to desired units or units specified in device's datasheet. 
    /// @return Converted value from voltage to desired units.
    virtual hal::result<float> get_parsed_data() = 0;
        
protected:
    hal::input_pin* digital_detector_ = nullptr;
    hal::adc* adc_data_;

};
} //namespace science