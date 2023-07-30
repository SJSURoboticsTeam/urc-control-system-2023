#pragma once

#include "../applications/application.hpp"
#include <libhal/servo.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/input_pin.hpp>
#include <libhal-util/units.hpp>
#include "offset_servo.hpp"


namespace sjsu::drive
{

// this function will return offsets in the same order of servos passed in
inline hal::status home(std::span<offset_servo*> p_servos,
                                        std::span<input_pin_homed> p_magnets,
                                        hal::steady_clock& p_counter) {

    using namespace std::chrono_literals;
    using namespace hal::literals;

    // if the sizes are not the same then return an error
    if (p_servos.size() != p_magnets.size()) {
        return hal::success();
    }

    bool going_to_60 = false;
    for (int i = 0; i < p_servos.size(); i++) {
        HAL_CHECK(p_servos[i]->position(0.0_deg));
        hal::delay(p_counter, 10ms);
    }

    // max time needed to move to 0 from anywhere at 2 rpms
    hal::delay(p_counter, 6s);

    // move them if 0 was home position due to inaccuracy
    for (int i = 0; i < p_servos.size(); i++) {
        if (!(HAL_CHECK(p_magnets[i].magnet->level()).state)) {
            p_servos[i]->m_offset = 60;
            HAL_CHECK(p_servos[i]->position(60.0_deg));
            going_to_60 = true;
        }
    }

    // then if it is going to 60 we need to delay the same amount as above
    if (going_to_60) {
        hal::delay(p_counter, 6s);
    }

    int number_of_legs_homed = 0;
    while (number_of_legs_homed != p_servos.size()) {

        for (int i = 0; i < p_servos.size(); i++) {

            if(!p_magnets[i].homed) {

                p_magnets[i].homed = !(HAL_CHECK(p_magnets[i].magnet->level()).state);
                hal::delay(p_counter, 10ms);

                if (p_magnets[i].homed) {
                    number_of_legs_homed++;
                    continue;
                } 
                p_servos[i]->m_offset++;
                HAL_CHECK(p_servos[i]->position(hal::degrees(p_servos[i]->m_offset)));
            }
            hal::delay(p_counter, 100ms);
        }
    }
    return hal::success();
}

}