#pragma once
#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>


//TODO: maybe later add CRC and config for altitude, ambient pressure, temperature offset (or look at setting persistent values)

namespace sjsu::science {
class scd40
{
private:

    scd40(hal::i2c& p_i2c, hal::steady_clock& p_clock);
    hal::i2c& m_i2c;
    hal::steady_clock& m_clock;
    
    //TODO: revise output type
    enum addresses {// deal with hal::byte later 
        device_address = 0x62,
        start_periodic_measurement_first_half = 0x21,
        start_periodic_measurement_second_half = 0xb1,
        read_measurement_first_half = 0xec,
        read_measurement_second_half = 0x05
    };

public:
    struct scd40_read_data
    {
        double co2;
        double temp;
        double rh;
    };

    static hal::result<scd40> create(hal::i2c& p_i2c,hal::steady_clock& p_clock);
    hal::status start();
    hal::result<scd40_read_data> read();
    hal::status stop();
};

} // namespace science