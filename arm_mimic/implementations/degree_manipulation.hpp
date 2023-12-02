#pragma once
#include <libhal-util/map.hpp>

namespace arm_mimic {

    hal::result<float> degree_phase_shift(float input_deg, float new_zero)
    {
        // if (new_zero < -360 || new_zero > 360)
        //     return hal::new_error("This function only considers the unit circle. Please set new_zero to the position on the unit circle it will be.");

        while (input_deg > 360 || input_deg < -360)
        {
            // take modulus of a float without losing data
            input_deg = input_deg / 360;
            input_deg = (input_deg - static_cast<int>(input_deg)) * 360;
        }

        return input_deg - new_zero;
    }

    float voltage_to_degree(float value, float max_voltage, float max_degree)
    {
        return hal::map(value,
                        std::pair<float, float>(0, max_voltage),
                        std::pair<float, float>(0, max_degree));
    }
}