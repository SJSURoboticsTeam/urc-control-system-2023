#pragma once

#include "../applications/application.hpp"
#include <libhal/servo.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/input_pin.hpp>
#include <libhal-util/units.hpp>


namespace sjsu::drive
{


// in order for this function to work, the size of the span of magnets and the servo_offsets need
// to be the same length, the magnets also have to be in the spans in the same order that they are
// in for the servo offsets or else this function will not work.
inline hal::status home(std::span<std::pair<hal::servo*, float>> p_servo_offsets, 
                        std::span<std::pair<hal::input_pin*, bool>> p_magnets,
                        hal::steady_clock& p_counter) {

    using namespace std::chrono_literals;
    using namespace hal::literals;

    // if the sizes are not the same then return an error
    // TODO: find out how to throw errors
    if (p_servo_offsets.size() != p_magnets.size()) {
      return hal::success();
    }


    bool going_to_60 = false;
    for(int i = 0; i < p_servo_offsets.size(); i++) {
      HAL_CHECK(p_servo_offsets[i].first.position(0.0_deg));
      hal::delay(p_counter, 10ms);
    }

    // max time needed to move to 0 from anywhere at 2 rpms
    hal::delay(p_counter, 6s);

    // move them if 0 was home position due to inacuracy
    for(int i = 0; i < p_servo_offsets.size(); i++) {
      if (!(HAL_CHECK(p_magnets[i].second.level()).state)) {
        p_servo[i].second = 60;
        HAL_CHECK(p_servo[i].first.position(60.0_deg));
        going_to_60 = true;
      }
    }

    // then if it is going to 60 we need to delay the same amount as above
    if (going_to_60) {
      hal::delay(p_counter, 6s);
    }

    int number_of_legs_homed = 0;
    while (number_of_legs_homed != p_servo_offsets.size()) {
      for(int i = 0; i < p_servo_offsets.size(); i++) {
        bool homed = p_magnets[i].second;

        if(!homed) {
          p_magnets[i].second = !(HAL_CHECK(p_magnets[i].level()).state);
          hal::delay(p_counter, 10ms);

          homed = p_magnets[i].second;
          if (homed) {
            all_are_homed++;
            continue;
          }

          float offset = p_servo_offsets[i].second++;
          HAL_CHECK(p_servo_offsets[i].first.position(hal::degrees(offset)));
        }
      hal::delay(p_counter, 100ms);
    }
    return hal::success();
    }
    
}


}