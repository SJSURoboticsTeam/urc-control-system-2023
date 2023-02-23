#pragma once

#include "generic_adc_sensor.hpp"

namespace science{

/// @brief Implementation for <SENSOR> ADC pressure sensor.
class PressureSensor : GenericAdcSensor {
public:
    /// @brief Builds pressure sensor driver
    /// @param adc The libhal ADC pin that has been initialized 
    PressureSensor(hal::adc& adc) : GenericAdcSensor(adc) {};

    /// @brief Reads pressure as detected by sensor NOTE: this function is a placeholder right now
    /// currently only returns raw voltage.
    /// @returns The current pressure in <UNITS> or a HAL error if reading has failed
    virtual hal::result<float> get_parsed_data() override {
        // TODO: implement proper conversion formula.
        return HAL_CHECK(read_raw_adc());
    } 
};
}
