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
    std::array<hal::byte, 2> start_address =  { start_periodic_measurement_first_half, start_periodic_measurement_second_half };
    HAL_CHECK(hal::write(m_i2c, addresses::device_address, start_address, hal::never_timeout()));
    return hal::success();
}

hal::result<scd40_nm::scd40_read_data> scd40_nm::read() {
    std::array<hal::byte, 2> read_address = {read_measurement_first_half, read_measurement_second_half };
    std::array<hal::byte, 9> buffer;

    HAL_CHECK(hal::write(m_i2c,addresses::device_address, read_address));
    hal::delay(m_clock, 1ms);
    HAL_CHECK(hal::read(m_i2c, addresses::device_address, buffer, hal::never_timeout()));
    
    scd40_nm::scd40_read_data rd;
    rd.co2 = buffer[0] << 8 | buffer[1];
    rd.temp = (-45 + 175.0*(buffer[3] << 8 | buffer[4])/ (1 << 16));
    rd.rh = 100.0 * (buffer[6] << 8 | buffer[7]) / (1 << 16);

    return rd;
}

