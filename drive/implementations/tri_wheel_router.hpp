#pragma once

#include <libhal-lpc40/input_pin.hpp>
#include <libhal/servo.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>

#include "../dto/drive.hpp"
#include "../dto/motor_feedback.hpp"
#include "../applications/application.hpp"

namespace sjsu::drive {
class tri_wheel_router
{
public:

  tri_wheel_router(leg& p_back, leg& p_right, leg& p_left)
    : m_left(p_left)
    , m_back(p_back)
    , m_right(p_right)
  {
  }

  hal::status move(tri_wheel_router_arguments p_tri_wheel_arguments,
                   hal::steady_clock& p_clock)
  {
    HAL_CHECK(m_left.steer->position(-p_tri_wheel_arguments.left.angle));
    HAL_CHECK(m_left.propulsion->power(-p_tri_wheel_arguments.left.speed));

    HAL_CHECK(m_right.steer->position(-p_tri_wheel_arguments.right.angle));
    HAL_CHECK(m_right.propulsion->power(-p_tri_wheel_arguments.right.speed));

    HAL_CHECK(m_back.steer->position(-p_tri_wheel_arguments.back.angle));
    HAL_CHECK(m_back.propulsion->power(-p_tri_wheel_arguments.back.speed));

    return hal::success();
  }

  /// At the moment, homing is where the legs turn on so we just calculate the
  /// initial encoder positions. ***Must be called in main
  // hal::status home(hal::steady_clock& counter)
  // {
  //   bool going_to_60 = false;

  //   HAL_CHECK(left_.steer_motor_.position_control(0.0_deg, 2.0_rpm));
  //   HAL_CHECK(hal::delay(counter, 10ms));
  //   HAL_CHECK(right_.steer_motor_.position_control(0.0_deg, 2.0_rpm));
  //   HAL_CHECK(hal::delay(counter, 10ms));
  //   HAL_CHECK(back_.steer_motor_.position_control(0.0_deg, 2.0_rpm));

  //   // max angle that the wheels will have to turn is 60 degrees for
  //   // this step, so 2rpm is 720deg/min which is 12 deg/sec which
  //   // means we need to wait 5 seconds to move 60 degrees but wait 6
  //   // seconds to be safe
  //   HAL_CHECK(hal::delay(counter, 6s));

  //   // these are active high
  //   bool leftPinLow = !(HAL_CHECK(left_.magnet_.level()).state),
  //        rightPinLow = !(HAL_CHECK(right_.magnet_.level()).state),
  //        backPinLow = !(HAL_CHECK(back_.magnet_.level()).state);

  //   HAL_CHECK(hal::delay(counter, 10ms));

  //   // TODO: put explanation for this V
  //   // move them if 0 was home position due to inacuracy
  //   if (leftPinLow) {
  //     left_.wheel_offset_ = 60;
  //     HAL_CHECK(left_.steer_motor_.position_control(60.0_deg, 2.0_rpm));
  //     going_to_60 = true;
  //   }
  //   if (rightPinLow) {
  //     right_.wheel_offset_ = 60;
  //     HAL_CHECK(right_.steer_motor_.position_control(60.0_deg, 2.0_rpm));
  //     going_to_60 = true;
  //   }
  //   if (backPinLow) {
  //     back_.wheel_offset_ = 60;
  //     HAL_CHECK(back_.steer_motor_.position_control(60.0_deg, 2.0_rpm));
  //     going_to_60 = true;
  //   }

  //   // then if it is going to 60 we need to delay the same amount as
  //   // the math above
  //   if (going_to_60) {
  //     HAL_CHECK(hal::delay(counter, 6s));
  //   }

  //   bool leftNotHome = true, rightNotHome = true, backNotHome = true;

  //   while (leftNotHome || rightNotHome || backNotHome) {
  //     if (leftNotHome) {
  //       leftNotHome = HAL_CHECK(step(left_, counter));
  //     }
  //     if (rightNotHome) {
  //       rightNotHome = HAL_CHECK(step(right_, counter));
  //     }
  //     if (backNotHome) {
  //       backNotHome = HAL_CHECK(step(back_, counter));
  //     }
  //     HAL_CHECK(hal::delay(counter, 100ms));
  //   }
  //   return hal::success();
  // }

  // hal::result<motor_feedback> get_motor_feedback(hal::steady_clock& clock)
  // {
  //   using namespace std::chrono_literals;
  //   using namespace hal::literals;
  //   motor_feedback motor_speeds;
  //   HAL_CHECK(
  //     left_.steer_motor_.feedback_request(hal::rmd::drc::read::status_2));
  //   HAL_CHECK(hal::delay(clock, 10ms));
  //   HAL_CHECK(
  //     right_.steer_motor_.feedback_request(hal::rmd::drc::read::status_2));
  //   HAL_CHECK(hal::delay(clock, 10ms));
  //   HAL_CHECK(
  //     back_.steer_motor_.feedback_request(hal::rmd::drc::read::status_2));
  //   HAL_CHECK(hal::delay(clock, 10ms));
  //   // theory: I don't think rmds return speed through set position
  //   // and only through set velocity, therefore we always have to
  //   // request for feedback from the steers
  //   motor_speeds.left_steer_speed = left_.steer_motor_.feedback().speed();
  //   motor_speeds.right_steer_speed = right_.steer_motor_.feedback().speed();
  //   motor_speeds.back_steer_speed = back_.steer_motor_.feedback().speed();
  //   motor_speeds.left_drive_speed = left_.drive_motor_.feedback().speed();
  //   motor_speeds.right_drive_speed = right_.drive_motor_.feedback().speed();
  //   motor_speeds.back_drive_speed = back_.drive_motor_.feedback().speed();
  //   return motor_speeds;
  // }

private:

  // member variables

  leg& m_left;
  leg& m_back;
  leg& m_right;
};
}  // namespace Drive