#pragma once

#include "../applications/application.hpp"
#include <libhal/servo.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/input_pin.hpp>
#include <libhal-util/units.hpp>


namespace sjsu::drive
{

// this function will return offsets in the same order of servos passed in
template <size_t number_of_legs>
inline hal::result<std::span<float>> home(std::span<hal::servo*> p_servos,
                                        std::span<hal::input_pin*> p_magnets,
                                        hal::steady_clock& p_counter) {

    using namespace std::chrono_literals;
    using namespace hal::literals;

    static std::array<float, number_of_legs> offsets{};
    std::array<bool, number_of_legs> homed{};
    homed.fill(false);
    
    // if the sizes are not the same then return an error
    if ((p_servos.size() != number_of_legs) && (p_magnets.size() != number_of_legs)) {
        return std::span<float>(offsets);
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
        if (!(HAL_CHECK(p_magnets[i]->level()).state)) {
            offsets[i] = 60;
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

            if(!homed[i]) {

                homed[i] = !(HAL_CHECK(p_magnets[i]->level()).state);
                hal::delay(p_counter, 10ms);

                if (homed[i]) {
                    number_of_legs_homed++;
                    continue;
                } 
                offsets[i]++;
                HAL_CHECK(p_servos[i]->position(hal::degrees(offsets[i])));
            }
            hal::delay(p_counter, 100ms);
        }
    }
    return std::span<float>(offsets);
}

}