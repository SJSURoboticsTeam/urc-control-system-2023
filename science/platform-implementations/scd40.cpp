// #pragma once

#include "scd40.hpp"

using namespace std::chrono_literals;
using scd40_nm = sjsu::science::scd40;


scd40_nm::scd40(hal::i2c& p_i2c, hal::steady_clock& p_clock) : m_i2c(p_i2c), m_clock(p_clock) {}

hal::result<scd40_nm> scd40_nm::create(hal::i2c& p_i2c, hal::steady_clock& clock){
    scd40 scd40(p_i2c, clock);
    HAL_CHECK(scd40.start());
    return scd40;

}

hal::status scd40_nm::start(){
    std::array<hal::byte, 2> start_address =  { start_first_half, start_second_half };
    HAL_CHECK(hal::write(m_i2c, addresses::device_address, start_address, hal::never_timeout()));
    return hal::success();
}

hal::result<std::array<hal::byte,9>> scd40_nm::read() {
    std::array<hal::byte, 2> read_address = {read_first_half, read_second_half };

    HAL_CHECK(hal::write(m_i2c,addresses::device_address, read_address));
    hal::delay(m_clock, 1ms);

    HAL_CHECK(hal::read(m_i2c, addresses::device_address, buffer, hal::never_timeout()));

    return buffer;
}

hal::result<double> scd40_nm::get_CO2(){
    read();
    return get_CO2_buffer();
}

hal::result<double> scd40_nm::get_temp(){
    read();
    return get_temp_buffer();
}

hal::result<double> scd40_nm::get_RH(){
    read();
    return get_RH_buffer();   
}

hal::result<double> scd40_nm::get_CO2_buffer(){
    return buffer[0] << 8 | buffer[1];
}

hal::result<double> scd40_nm::get_temp_buffer(){
    return (-45 + 175.0*(buffer[3] << 8 | buffer[4])/ (1 << 16));
}

hal::result<double> scd40_nm::get_RH_buffer(){
    return 100.0 * (buffer[7] << 8 | buffer[8]) / (1 << 16);   
}
