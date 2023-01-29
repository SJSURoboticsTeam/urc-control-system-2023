#pragma once

// changed -> libhal

#include <cstdint>
#include <cmath>

#include <libhal-rmd/drc.hpp>
#include <libhal-util/map.hpp>

namespace Drive {
class RmdEncoder
{
// public:
//   static float CalcEncoderPositions(hal::rmd::drc& motor, float type)
//   {
//     using namespace hal::literals;
//     motor.feedback_request(hal::rmd::drc::read::encoder_data);
//     int eight_bit_raw_encoder_data =
//       static_cast<float>(motor.feedback().encoder);
//     // We shift right because the driver returns this value as a 16-bit number
//     // but the datasheet says its an 8 bit number
//     return MapEncoderDataToDegrees(eight_bit_raw_encoder_data >> 8, type);
//   }

// private:
//   static float MapEncoderDataToDegrees(int16_t encoder_data, float type)
//   {
//     // we use a float here because we need decimal precision
//     float temp = (type * encoder_data / 256.0f);
//     return temp;
//   }
public:
      static float CalcEncoderPositions(hal::rmd::drc& motor)
      {
        //We shift right because the driver returns this value as a 16-bit number but the datasheet says its an 8 bit number

        motor.feedback_request(hal::rmd::drc::read::encoder_data);

        return MapEncoderDataToDegrees(motor.feedback().encoder);
      }
private:
      static float MapEncoderDataToDegrees(float encoder_data)
      {
          // sjsu::LogInfo("%d", encoder_data);
          static constexpr std::uint32_t max_value = (1 << 15) - 1;
          static constexpr auto max_value_float = static_cast<float>(max_value);
          
          float temp = (60.0f * (encoder_data) / max_value_float);
          // sjsu::LogInfo("%d", temp);
          return temp;
      }

};
// };
}  // namespace Drive