#pragma once

#include "generic_adc_sensor.hpp"

namespace science{

/// @brief Implementation for <SENSOR> ADC pressure sensor.
class PressureSensor {
public:
    /// @brief Builds pressure sensor driver
    /// @param adc The libhal ADC pin that has been initialized 
    PressureSensor(hal::adc& adc) : adc_(adc) {};

    /// @brief Reads pressure as detected by sensor NOTE: this function is a placeholder right now
    /// currently only returns raw voltage.
    /// @returns The current pressure in <UNITS> or a HAL error if reading has failed
    hal::result<float> get_parsed_data() {
        // TODO: implement proper conversion formula.
        return HAL_CHECK(read_raw_adc());
    }
    
    hal::result<float> read_raw_adc() {
        float raw_ratio_average = 0;
        int read_count = 10;
        for (int i = 0; i < read_count; i++)
        {
            raw_ratio_average += HAL_CHECK(adc_.read()).sample;
        }
        
        raw_ratio_average /= read_count;

        return raw_ratio_average;
    }

    /// @brief Detects if pressure sensor is experiencing ambient pressure (That is atmospheric pressure give or take 1 AMS)
    /// NOTE: This function is a placeholder, it will always return an error and fail HAL_CHECKs.
    /// @return True if the environment around the sensor is ambient.
    hal::result<bool> is_pressurized() {
        return hal::new_error("implement a way to detect a pressureized or depressureized chamber, will depend on sensor specs and how its powered.");
        return false;
    }
    private: 
    hal::adc& adc_;    
};
}
