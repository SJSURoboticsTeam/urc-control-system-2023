#pragma once

#include "../applications/application.hpp"
#include <libhal/servo.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/input_pin.hpp>
#include <libhal-util/units.hpp>
#include "offset_servo.hpp"

namespace sjsu::drive
{
struct homing {
  offset_servo* servo;
  hal::input_pin* magnet;
  bool homed = false;
};

// this function will return offsets in the same order of servos passed in
inline hal::status home(std::span<homing*> p_homing_structs,
                        hal::steady_clock& p_counter,
                        hal::serial* terminal) {

    using namespace std::chrono_literals;
    using namespace hal::literals;

    bool going_to_60 = false;
    HAL_CHECK(hal::write(*terminal, "this many legs\n"));
    for (int i = 0; i < p_homing_structs.size(); i++) {
        HAL_CHECK(p_homing_structs[i]->servo->position(0.0_deg));
        HAL_CHECK(hal::write(*terminal, "going to 0\n"));
        hal::delay(p_counter, 10ms);
    }

    // max time needed to move to 0 from anywhere at 2 rpms
    hal::delay(p_counter, 6s);

    // move them if 0 was home position due to inaccuracy
    HAL_CHECK(hal::write(*terminal, "set back to 60 deg\n"));
    for (int i = 0; i < p_homing_structs.size(); i++) {
        if (!(HAL_CHECK(p_homing_structs[i]->magnet->level()).state)) {
            p_homing_structs[i]->servo->set_offset(60.0_deg);
            HAL_CHECK(p_homing_structs[i]->servo->position(60.0_deg));
            going_to_60 = true;
            HAL_CHECK(hal::write(*terminal, "setting back to 60\n"));
        }
    }

    // then if it is going to 60 we need to delay the same amount as above
    if (going_to_60) {
        hal::delay(p_counter, 6s);
    }

    int number_of_legs_homed = 0;
    while (number_of_legs_homed != p_homing_structs.size()) {
        HAL_CHECK(hal::write(*terminal, "In the While\n"));
        for (int i = 0; i < p_homing_structs.size(); i++) {
            HAL_CHECK(hal::write(*terminal, "in the for\n"));
            if(!p_homing_structs[i]->homed) {

                p_homing_structs[i]->homed = !(HAL_CHECK(p_homing_structs[i]->magnet->level()).state);
                hal::delay(p_counter, 10ms);
                if (p_homing_structs[i]->homed) {
                    number_of_legs_homed++;
                    HAL_CHECK(hal::write(*terminal, "number of legs homed\n"));
                    continue;
                } 
                auto new_offset = p_homing_structs[i]->servo->get_offset() + 1.0_deg;
                p_homing_structs[i]->servo->set_offset(new_offset);
                HAL_CHECK(p_homing_structs[i]->servo->position(hal::degrees(new_offset)));
            }
            hal::delay(p_counter, 100ms);
        }
    }
    return hal::success();
}

}