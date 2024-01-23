#pragma once

#include <libhal-util/i2c.hpp>
#include <libhal-util/serial.hpp>
#include "scd40.hpp"

scd40::create(hal::i2c& p_i2c, hal::steady_clock& clock){
    scd40 scd40 = scd40(p_i2c, clock);
    HAL_CHECK(scd40.start())

}

scd40::start(){}

scd40::get_CO2{}

scd40::get_RH(){}

scd40::get_temp(){}