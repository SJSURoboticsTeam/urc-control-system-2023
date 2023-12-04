#pragma once

#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>


namespace hal::sht {
  class sht21 {
    public:
      const static hal::byte sht21_i2c_address = 0x40;

      static hal::result<sht21> create(hal::i2c * bus, hal::byte p_address = sht21_i2c_address);
      
      hal::status soft_reset();

      hal::result<bool> is_low_battery();
      hal::status enable_heater(bool enabled = true);
      hal::status enable_otp(bool enabled = true);

      hal::result<double> get_relative_humidity();
      hal::result<double> get_temperature();

    private:

      enum commands {
        hold_temperature_measurement = 0b11100011,
        hold_relative_humidity_measurement = 0b11100101,
        no_hold_temperature_measurement = 0b11110011,
        no_hold_relative_humidity_measurement = 0b11110101,

        write_user_register = 0b11100110,
        read_user_register = 0b11100111,

        soft_reset = 0b11111110,
      };

      hal::status execute_command(commands command);

      /**
       * @brief I2C Bus
       */
      hal::i2c* m_i2c;

      /**
       * @brief Device address
       */
      hal::byte m_address;
  
      sht21(hal::i2c * p_bus, hal::byte p_address);
  };
};