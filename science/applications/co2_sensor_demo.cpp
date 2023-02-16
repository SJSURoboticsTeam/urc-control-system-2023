#pragma once

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>
#include <libhal/units.hpp>

#include "../hardware_map.hpp"
#include "../implementations/co2_sensor.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map) {
    // science robot entry point here. This function may yield an error.
    // configure drivers

    while (true) {
        // demo for methane sensor driver.
        science::Co2Sensor co2_driver = science::Co2Sensor(*p_map.carbon_dioxide_sensor);

        hal::byte is_co2 = HAL_CHECK(co2_driver.read_co2());
        // float methane_value = HAL_CHECK(c02_driver.read());

        // if (is_methane) {
        //     hal::print<64>(*p_map.science_serial, "CH4: %f\n", methane_value);
        // } else {
        //     hal::print<64>(*p_map.science_serial, "No methane detected.\n");
        // }
        hal::print<64>(*p_map.science_serial, "C02: %d\n", is_co2);

        HAL_CHECK(hal::delay(*p_map.clock, 100ms));

    }

    return hal::success();
}
