#pragma once

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "../libhal_configuration/hardware_map.hpp"
#include "../implementations/mq4_methane_sensor.hpp"


using namespace hal::literals;
using namespace std::chrono_literals;

namespace demos {

hal::status methane_sensor_demo(science::hardware_map& p_map) {
    // science robot entry point here. This function may yield an error.
    // configure drivers

    while (true) {
        // demo for methane sensor driver.
        Mq4MethaneSensor methane_driver = Mq4MethaneSensor(p_map.methane_level, p_map.is_methane);

        bool is_methane = HAL_CHECK(methane_driver.detect_signal());
        float methane_value = HAL_CHECK(methane_driver.read_data());

        if (is_methane) {
            hal::print<64>(*p_map.science_serial, "CH4: %f\n", methane_value);
        } else {
            hal::print<64>(*p_map.science_serial, "No methane detected.\n");
        }
        HAL_CHECK(hal::delay(*p_map.clock, 100ms));

    }

    return hal::success();
}

} // namespace demos
