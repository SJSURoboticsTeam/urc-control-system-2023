#pragma once

#include "../applications/application.hpp"
#include "../include/offset_servo.hpp"
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal/input_pin.hpp>
#include <libhal/servo.hpp>

namespace sjsu::drive {
struct homing
{
  offset_servo* servo;
  hal::input_pin* magnet;
  bool homed = false;
};

// this function will return offsets in the same order of servos passed in
inline void home(std::span<homing*> p_homing_structs,
                        hal::steady_clock& p_counter)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  bool going_to_60 = false;
  for (int i = 0; i < p_homing_structs.size(); i++) {
    HAL_CHECK(p_homing_structs[i]->servo->position(0.0_deg));
    hal::delay(p_counter, 2ms);
  }

  // max time needed to move to 0 from anywhere at 2 rpms
  hal::delay(p_counter, 6s);

  // move them if 0 was home position due to inaccuracy
  for (int i = 0; i < p_homing_structs.size(); i++) {
    if (!(HAL_CHECK(p_homing_structs[i]->magnet->level()).state)) {
      p_homing_structs[i]->servo->set_offset(60.0_deg);
      HAL_CHECK(p_homing_structs[i]->servo->position(60.0_deg));
      going_to_60 = true;
    }
  }

  // then if it is going to 60 we need to delay the same amount as above
  if (going_to_60) {
    hal::delay(p_counter, 6s);
  }

  int number_of_legs_homed = 0;
  while (number_of_legs_homed != p_homing_structs.size()) {
    for (int i = 0; i < p_homing_structs.size(); i++) {
      if (!p_homing_structs[i]->homed) {

        p_homing_structs[i]->homed =
          !(HAL_CHECK(p_homing_structs[i]->magnet->level()).state);
        hal::delay(p_counter, 2ms);
        if (p_homing_structs[i]->homed) {
          number_of_legs_homed++;
          continue;
        }
        auto new_offset = p_homing_structs[i]->servo->get_offset() + 1.0_deg;
        p_homing_structs[i]->servo->set_offset(new_offset);
        HAL_CHECK(p_homing_structs[i]->servo->position(0.0_deg));
      }
      hal::delay(p_counter, 60ms);
    }
  }
  return hal::success();
}

}  // namespace sjsu::drive
