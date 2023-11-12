#pragma once

#include "scd40.hpp"

using scd40 = science::scd40;
scd40::scd40(hal::i2c& p_i2c, hal::steady_clock& p_clock) : m_i2c(p_i2c), m_clock(p_clock) {}

hal::result<scd40> scd40::create(hal::i2c& p_i2c, hal::steady_clock& clock){
    scd40 scd40 = science::scd40(p_i2c, clock);
    HAL_CHECK(scd40.start());
    return scd40;

}

hal::status scd40::start(){
    std::array<hal::byte, 2> start_address =  { start_first_half, start_second_half };
    HAL_CHECK(hal::write(m_i2c, device_address, start_address, hal::never_timeout()));
    return hal::success();
}

std::array<hal::byte,9> scd40::read() {
    std::array<hal::byte, 2> read_address = {read_first_half, read_second_half };
    std::array<hal::byte, 9> buffer;

    HAL_CHECK(hal::write(m_i2c, device_address, read_address));
    HAL_CHECK(hal::delay(m_clock, 1ms));

    HAL_CHECK(hal::read(m_i2c, addresses::address, buffer, hal::never_timeout()));

    return buffer;
}
// returns in ppm
hal::result<double> scd40::get_CO2(){
    std::array<hal::byte, 9> readResults = read();
    return ((readResults[0] << 8) | readResults[1]);
}

hal::result<double> scd40::get_temp(){
    std::array<hal::byte, 9> buffer = read();
    return (-45 + 175*(buffer[3] << 8 | buffer[4])/ 65536);
}

hal::result<double> scd40::get_RH(){
    std::array<hal::byte, 9> buffer = read();
    return 100 * (buffer[7] << 8 | buffer[8]) /65536;
    
}
