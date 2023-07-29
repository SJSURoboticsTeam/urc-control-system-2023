#pragma once

#include "../applications/application.hpp"
#include <libhal/servo.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/input_pin.hpp>
#include <libhal-util/units.hpp>


namespace sjsu::drive
{

inline hal::result<bool> step(hal::servo& p_servo, hal::input_pin& p_magnet, int& p_servo_offset, hal::steady_clock& p_counter)
  {
    using namespace std::chrono_literals;
    using namespace hal::literals;
    // level returns true if it is high, and the magnet is high when
    // it is not
    // ***********NOTE THIS MAY NOT WORK
    bool homed = !(HAL_CHECK(p_magnet.level()).state);
    hal::delay(p_counter, 10ms);

    if (homed) {
      return false;
    }
    p_servo_offset++;
    HAL_CHECK(p_servo.position(hal::degrees(p_servo_offset)));
    return true;
  }

inline hal::status home(hal::servo& p_back_servo, hal::servo& p_right_servo, hal::servo& p_left_servo, 
    hal::input_pin& p_back_magnet, hal::input_pin& p_right_magnet, hal::input_pin& p_left_magnet, hal::steady_clock& p_counter) {

    using namespace std::chrono_literals;
    using namespace hal::literals;

   int back_wheel_offset = 0, right_wheel_offset = 0, left_wheel_offset = 0;
   bool going_to_60 = false;

    HAL_CHECK(p_left_servo.position(0.0_deg));
    hal::delay(p_counter, 10ms);
    HAL_CHECK(p_right_servo.position(0.0_deg));
    hal::delay(p_counter, 10ms);
    HAL_CHECK(p_back_servo.position(0.0_deg));

    // max angle that the wheels will have to turn is 60 degrees for
    // this step, so 2rpm is 720deg/min which is 12 deg/sec which
    // means we need to wait 5 seconds to move 60 degrees but wait 6
    // seconds to be safe
    hal::delay(p_counter, 6s);

    // these are active high
    bool leftPinLow = !(HAL_CHECK(p_left_magnet.level()).state),
         rightPinLow = !(HAL_CHECK(p_right_magnet.level()).state),
         backPinLow = !(HAL_CHECK(p_back_magnet.level()).state);

    hal::delay(p_counter, 10ms);

    // TODO: put explanation for this V
    // move them if 0 was home position due to inacuracy
    if (leftPinLow) {
      left_wheel_offset = 60;
      HAL_CHECK(p_left_servo.position(60.0_deg));
      going_to_60 = true;
    }
    if (rightPinLow) {
      right_wheel_offset = 60;
      HAL_CHECK(p_right_servo.position(60.0_deg));
      going_to_60 = true;
    }
    if (backPinLow) {
      back_wheel_offset = 60;
      HAL_CHECK(p_back_servo.position(60.0_deg));
      going_to_60 = true;
    }

    // then if it is going to 60 we need to delay the same amount as
    // the math above
    if (going_to_60) {
      hal::delay(p_counter, 6s);
    }

    bool leftNotHome = true, rightNotHome = true, backNotHome = true;

    while (leftNotHome || rightNotHome || backNotHome) {
      if (leftNotHome) {
        leftNotHome = HAL_CHECK(step(p_left_servo, p_left_magnet, left_wheel_offset, p_counter));
      }
      if (rightNotHome) {
        rightNotHome = HAL_CHECK(step(p_right_servo, p_right_magnet, right_wheel_offset, p_counter));
      }
      if (backNotHome) {
        backNotHome = HAL_CHECK(step(p_back_servo, p_back_magnet, back_wheel_offset, p_counter));
      }
      hal::delay(p_counter, 100ms);
    }
    return hal::success();
  
}


}