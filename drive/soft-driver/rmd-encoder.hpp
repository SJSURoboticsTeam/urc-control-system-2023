#pragma once

// changed -> libhal

#include <cstdint>
#include <cmath>

#include <libhal-rmd/drc.hpp>
#include <libhal-util/map.hpp>

namespace Drive {
class RmdEncoder
{
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