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
    hal::print<64>(*p_map.science_serial, "before while loop");
    science::Co2Sensor co2_driver = science::Co2Sensor(*p_map.carbon_dioxide_sensor);
    hal::print(*p_map.science_serial, "after initialization");
    // hal::byte co2 = HAL_CHECK(co2_driver.read_co2());

    while (true) {
        // hal::print(*p_map.science_serial, "inside while loop");
        hal::byte co2 = HAL_CHECK(co2_driver.read_co2());

    //     hal::print(*p_map.science_serial, "pls work");

        // possibly convert from hex first
        // hal::print(*p_map.science_serial, co2);
        
        hal::print<64>(*p_map.science_serial, "C02: %d\n", co2);

        HAL_CHECK(hal::delay(*p_map.clock, 100ms));

    }

    return hal::success();
}
