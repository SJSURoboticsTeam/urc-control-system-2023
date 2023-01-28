#pragma once
#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/units.hpp>

#include <libhal/i2c.hpp>

using namespace std::chrono_literals;
using namespace hal::literals;
namespace Arm {
class PCA9685
{
public:
  struct settings
  {
    hal::hertz frequency = 50.0_Hz;
    hal::degrees min_angle = 0.0_deg;
    hal::degrees max_angle = 180.0_deg;
    hal::time_duration min_pulse = 520us;
    hal::time_duration max_pulse = 2400us;
  };

  static constexpr hal::byte kModeRegister1 = 0x00;
  static constexpr hal::byte kAllOutputAddress = 0x05;
  static constexpr hal::byte kOutputAddress0 = 0x06;
  static constexpr hal::byte kPreScaler = 0xFE;

  PCA9685(hal::i2c& p_i2c, uint8_t p_address)
    : m_i2c(p_i2c)
    , m_address(p_address)
  {
    enableDevice();
  }

  void setAngle(uint8_t p_output_number, hal::degrees p_angle)
  {
  }

private:
  void setFrequency(hal::hertz p_output_frequency = 50.0_Hz)
  {
    hal::write(
      m_i2c, m_address, std::array{ kPreScaler }, hal::never_timeout());
  }

  void enableDevice()
  {
    hal::byte mode_register_data;
    constexpr auto kSleepMask = hal::bit::mask{ 4 };

    // auto response = std::array<hal::byte, 1>{};
    // hal::write_then_read(m_i2c,
    //                      m_address,
    //                      std::array{ kModeRegister1 },
    //                      response,
    //                      hal::never_timeout());
    // hal::write(m_i2c,
    //            m_address,
    //            std::array{ kModeRegister1, hal::byte(0x01) },
    //            hal::never_timeout());
  }

  void setPulseWidthModulation(uint8_t p_output_number,
                               float p_duty_cycle,
                               float p_delay = 0.0f)
  {
  }

  hal::i2c& m_i2c;
  const uint8_t m_address;
};
}  // namespace Arm