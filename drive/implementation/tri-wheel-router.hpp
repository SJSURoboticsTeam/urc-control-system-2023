#pragma once

// changed -> libhal

#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/units.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../dto/drive-dto.hpp"
#include "../dto/motor-feedback-dto.hpp"
#include "../soft-driver/rmd-encoder.hpp"

namespace Drive {
class TriWheelRouter
{
public:
  struct leg
  {
    leg(hal::rmd::drc& steer, hal::rmd::drc& drive, hal::input_pin& magnet)
      : steer_motor_(steer)
      , drive_motor_(drive)
      , magnet_(magnet)
    {
    }
    hal::rmd::drc& steer_motor_;
    hal::rmd::drc& drive_motor_;
    hal::input_pin& magnet_;
    int16_t wheel_offset_ = 0;
  };

  TriWheelRouter(leg right, leg left, leg back)
    : left_(left)
    , back_(back)
    , right_(right)
  {
  }

  tri_wheel_router_arguments SetLegArguments(
    tri_wheel_router_arguments tri_wheel_arguments)
  {
    left_.steer_motor_.position_control(
      hal::degrees(-tri_wheel_arguments.left.steer.angle + left_.wheel_offset_),
      hal::rpm(tri_wheel_arguments.left.steer.speed));
    left_.drive_motor_.velocity_control(
      -hal::rpm(tri_wheel_arguments.left.hub.speed));

    right_.steer_motor_.position_control(
      hal::degrees(-tri_wheel_arguments.right.steer.angle + right_.wheel_offset_),
      hal::rpm(tri_wheel_arguments.right.steer.speed));
    right_.drive_motor_.velocity_control(
      -hal::rpm(tri_wheel_arguments.right.hub.speed));

    back_.steer_motor_.position_control(
      hal::degrees(-tri_wheel_arguments.back.steer.angle + back_.wheel_offset_),
      hal::rpm(tri_wheel_arguments.back.steer.speed));
    back_.drive_motor_.velocity_control(
      -hal::rpm(tri_wheel_arguments.back.hub.speed));

    tri_wheel_arguments_ = tri_wheel_arguments;
    return tri_wheel_arguments_;
  }

  tri_wheel_router_arguments GetTriWheelRouterArguments() const
  {
    return tri_wheel_arguments_;
  }

  /// At the moment, homing is where the legs turn on so we just calculate the
  /// initial encoder positions. ***Must be called in main
  hal::status HomeLegs(hal::serial& terminal, hal::steady_clock& counter)
  {
    using namespace std::chrono_literals;
    using namespace hal::literals;
    bool going_to_60 = false;

    left_.steer_motor_.position_control(0.0_deg, 2.0_rpm);
    HAL_CHECK(hal::delay(counter, 10ms));
    right_.steer_motor_.position_control(0.0_deg, 2.0_rpm);
    HAL_CHECK(hal::delay(counter, 10ms));
    back_.steer_motor_.position_control(0.0_deg, 2.0_rpm);

    // max angle that the wheels will have to turn is 60 degrees for this step, so 2rpm is 720deg/min
    // which is 12 deg/sec which means we need to wait 5 seconds to move 60 degrees but wait 6 seconds to be safe
    HAL_CHECK(hal::delay(counter, 6s));

    // these are active high
    bool leftPinLow = !(HAL_CHECK(left_.magnet_.level())),
         rightPinLow = !(HAL_CHECK(right_.magnet_.level())),
         backPinLow = !(HAL_CHECK(back_.magnet_.level()));
      
    HAL_CHECK(hal::delay(counter, 10ms));

    if(leftPinLow) 
    {
      left_.wheel_offset_ = 60;
      left_.steer_motor_.position_control(60.0_deg, 2.0_rpm);
      going_to_60 = true;
    }
    if(rightPinLow) 
    {
      right_.wheel_offset_ = 60;
      right_.steer_motor_.position_control(60.0_deg, 2.0_rpm);
      going_to_60 = true;
    }
    if(backPinLow) 
    {
      back_.wheel_offset_ = 60;
      back_.steer_motor_.position_control(60.0_deg, 2.0_rpm);
      going_to_60 = true;
    }
    // move them if 0 was home position due to inacuracy
    // then if it is going to 60 we need to delay the same amount as the math above
    if(going_to_60) {
      HAL_CHECK(hal::delay(counter, 6s));
    }

    bool leftNotHome = true, rightNotHome = true, backNotHome = true;

    while (leftNotHome || rightNotHome || backNotHome) {
      if(leftNotHome) {
        leftNotHome = HAL_CHECK(WheelNotHomeDoThis(left_, counter));
      }
      if(rightNotHome) {
        rightNotHome = HAL_CHECK(WheelNotHomeDoThis(right_, counter));
      }
      if(backNotHome) {
        backNotHome = HAL_CHECK(WheelNotHomeDoThis(back_, counter));
      }

      hal::print<50>(terminal, "Homing Pins: L = %d | ", HAL_CHECK(left_.magnet_.level()));
      hal::print<50>(terminal, "R = %d | ", HAL_CHECK(right_.magnet_.level()));
      hal::print<50>(terminal, "B = %d ", HAL_CHECK(back_.magnet_.level()));
      hal::print<50>(terminal, "Wheel Offset: L = %d | ", static_cast<int>(left_.wheel_offset_));
      hal::print<50>(terminal, "R = %d | ", static_cast<int>(right_.wheel_offset_));
      hal::print<50>(terminal, "B = %d \n", static_cast<int>(back_.wheel_offset_));

      HAL_CHECK(hal::delay(counter, 100ms));
    }
    return hal::success();
  }

  hal::result<motor_feedback> GetMotorFeedback()
  {
    motor_feedback motor_speeds;
    // Creating this enum from the drc class allows us to read all data from the
    // rmd when it is passed into the feedback_request function
    hal::rmd::drc::read read_commands;

    HAL_CHECK(left_.steer_motor_.feedback_request(read_commands));
    HAL_CHECK(right_.steer_motor_.feedback_request(read_commands));
    HAL_CHECK(left_.steer_motor_.feedback_request(read_commands));

    motor_speeds.left_steer_speed = left_.steer_motor_.feedback().speed();
    motor_speeds.right_steer_speed = right_.steer_motor_.feedback().speed();
    motor_speeds.back_steer_speed = back_.steer_motor_.feedback().speed();
    return motor_speeds;
  }

private:

  hal::result<bool> WheelNotHomeDoThis(leg& leg_, hal::steady_clock& counter)
  {
    using namespace std::chrono_literals;
    using namespace hal::literals;

    // level returns true if it is high, and the magnet is high when it is not
    bool not_homed = HAL_CHECK(leg_.magnet_.level());
    HAL_CHECK(hal::delay(counter, 10ms));

    if (not_homed) {
      leg_.wheel_offset_++;
      leg_.steer_motor_.position_control(hal::degrees(leg_.wheel_offset_), 2.0_rpm);
      return true;
    } 
    else {
      return false;
    }
  }

  // member variables

  leg left_;
  leg back_;
  leg right_;
  tri_wheel_router_arguments tri_wheel_arguments_;
};
}  // namespace Drive