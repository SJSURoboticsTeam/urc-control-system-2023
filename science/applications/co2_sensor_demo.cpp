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
    auto& carbon_dioxide_sensor = *p_map.carbon_dioxide_sensor;
    auto& clock = p_map.clock;
    // hal::print<64>(*p_map.science_serial, "before co2 initializng");
    
    auto co2_driver = HAL_CHECK(science::Co2Sensor::create(carbon_dioxide_sensor, clock));
    // hal::print<64>(*p_map.science_serial, "after co2 initializng");

    // hal::print<64>(*p_map.science_serial, "before while loop");

    // science::Co2Sensor co2_driver = science::Co2Sensor(carbon_dioxide_sensor, clock);

    // hal::print(*p_map.science_serial, "after initialization");
    // hal::byte co2 = HAL_CHECK(co2_driver.read_co2());

    while (true) {
        HAL_CHECK(hal::delay(*p_map.clock, 500ms));

        // hal::print(*p_map.science_serial, "inside while loop");
        auto co2 = HAL_CHECK(co2_driver.read_co2());

    //     hal::print(*p_map.science_serial, "pls work");

        // possibly convert from hex first
        // hal::print(*p_map.science_serial, co2);
        
        hal::print<64>(*p_map.science_serial, "C02: %d\n", co2);
        // hal::print<64>(*p_map.science_serial, "C02: %d\n", co2[1]);
        // hal::print<64>(*p_map.science_serial, "C02: %d\n", co2[2]);

        // hal::print<64>(*p_map.science_serial, "T %d\n", co2[3]);

        // hal::print<64>(*p_map.science_serial, "T %d\n", co2[4]);

        // hal::print<64>(*p_map.science_serial, "T %d\n", co2[5]);

        // hal::print<64>(*p_map.science_serial, "H: %d\n", co2[6]);

        // hal::print<64>(*p_map.science_serial, "H %d\n", co2[7]);
        // hal::print<64>(*p_map.science_serial, "H %d\n", co2[8]);

        HAL_CHECK(hal::delay(*p_map.clock, 1000ms));

    }

    return hal::success();
}
