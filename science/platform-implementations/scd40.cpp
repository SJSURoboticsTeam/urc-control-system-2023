#pragma once

#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include "scd40.hpp"

class scd40 {
    create(hal::i2c& p_i2c, hal::steady_clock& clock){
        scd40 scd40 = scd40(p_i2c, clock);
        HAL_CHECK(scd40.start())

    }

    start(){
        std::array<hal::byte, 2> start_address =  { start_first_half, start_second_half };
        HAL_CHECK(hal::write(m_i2c, device_address, start_address, hal::never_timeout()));
        return hal::success();
    }

    std::array<hal::byte,9> read() {
        std::array<const hal::byte, 2> read_address = {read_first_half, read_second_half };
        std::array<hal::byte, 9> buffer;

        HAL_CHECK(hal::write(m_i2c, device_address, read_address));
        HAL_CHECK(hal::delay(steady_clock, 1ms));

        HAL_CHECK(
        hal::read(m_i2c, addresses::address, buffer, hal::never_timeout()));
        return buffer;
    }

    get_CO2(){}

    get_RH(){}

    get_temp(){}
}