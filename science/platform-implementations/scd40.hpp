#pragma once

#include <"scd40.cpp">

//TODO: maybe later add CRC and config for altitude, ambient pressure, temperature offset (or look at setting persistent values)

namespace science {
class scd40
{
private:
    scd40(hal::i2c& p_i2c, hal::steady_clock& p_clock);
    enum addresses hal::byte {
        device_address = 0x62,
        start_first_half = 0x21,
        start_second_half = 0xb1,
        read_first_half = 0xec,
        read_second_half = 0x05
    };

public:
    static hal::result<co2_sensor> create(hal::i2c& p_i2c,hal::steady_clock& p_clock);
    hal::status start();
    
    //TODO: revise output type
    
    hal::result start();
    hal::result<double> get_CO2();
    hal::result<double> get_RH();
    hal::result<double> get_temp();
};

}