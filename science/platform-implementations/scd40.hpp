#pragma once
#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>


//TODO: maybe later add CRC and config for altitude, ambient pressure, temperature offset (or look at setting persistent values)

namespace science {
class scd40
{
private:

    scd40(hal::i2c& p_i2c, hal::steady_clock& p_clock);
    enum addresses  {// deal with hal::byte later 
        device_address = 0x62,
        start_first_half = 0x21,
        start_second_half = 0xb1,
        read_first_half = 0xec,
        read_second_half = 0x05
    };

public:
    static hal::result<scd40> create(hal::i2c& p_i2c,hal::steady_clock& p_clock);
    hal::status start();
    std::array<hal::byte,9> read();
    
    //TODO: revise output type
    hal::result<double> get_CO2();
    hal::result<double> get_RH();
    hal::result<double> get_temp();
};

} // namespace science