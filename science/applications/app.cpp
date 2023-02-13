#pragma once
#include "../implementations/mq4_methane_sensor.hpp"
#include "../implementations/pressure_sensor_driver.hpp"
#include "../implementations/pump_controller.hpp"

hal::status application(science::hardware_map &p_map) {
    using namespace std::chrono_literals;
    using namespace hal::literals;
    int automatic = 1;
    while(true) {
        // Get mission control data here
        if(automatic == 1) {
            
        }
        else if(automatic == 0) {
            // do alternative loop
        }
    }
    return hal::success();
}
