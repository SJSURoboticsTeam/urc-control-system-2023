#pragma once

// changed -> libhal

#include <libhal-rmd/drc.hpp>

namespace Drive {
class RmdEncoder
{
public:
  static float CalcEncoderPositions(hal::rmd::drc& motor, float type)
  {
    using namespace hal::literals;
    motor.feedback_request(hal::rmd::drc::read::encoder_data);
    int eight_bit_raw_encoder_data =
      static_cast<float>(motor.feedback().encoder);
    // We shift right because the driver returns this value as a 16-bit number
    // but the datasheet says its an 8 bit number
    return MapEncoderDataToDegrees(eight_bit_raw_encoder_data, type);
  }

private:
  static float MapEncoderDataToDegrees(int16_t encoder_data, float type)
  {
    // we use a float here because we need decimal precision
    float temp = (type * encoder_data / 256.0f);
    return temp;
  }
};
}  // namespace Drive