#pragma once

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "../hardware_map.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map) {
    // science robot entry point here. This function may yield an error.
    // configure drivers

    while (true) {

        //read from the halleffect sensor
        auto result = HAL_CHECK(p_map.revolver_hall_effect->level());

        //display the results every second. 1 means sensor detects no magnet. 0 means sensor detects magnet.
        hal::print<64>(*p_map.terminal, "hall effect sensor: %d\n", result);
        HAL_CHECK(hal::delay(*p_map.clock, 100ms));
        
    }

    return hal::success();
}
