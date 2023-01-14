#pragma once

// changed -> libhal

#include <librmd/drc.hpp>

namespace Drive
{
    class RmdEncoder {
    public:
        static float CalcEncoderPositions(hal::rmd::drc &motor, float type)
        {
            hal::rmd::drc::read read_commands;
            HAL_CHECK(motor.steer_motor_.feedback_request(read_commands));
            int eight_bit_raw_encoder_data = static_cast<float>(motor.steer_motor_.feedback().encoder >> 8)
            //We shift right because the driver returns this value as a 16-bit number but the datasheet says its an 8 bit number
                return MapEncoderDataToDegrees(eight_bit_raw_encoder_data, type);
        }
    private:
        static float MapEncoderDataToDegrees(int8_t encoder_data, float type)
        {
            // we use a float here because we need decimal precision
            float temp = (type * encoder_data / 256.0f);
            return temp;
        }

    };
}