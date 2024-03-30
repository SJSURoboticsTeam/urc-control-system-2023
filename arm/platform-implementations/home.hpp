#include <libhal-rmd/mc_x.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/accelerometer.hpp>
#include <libhal/error.hpp>
#include <math.h>

using namespace std::chrono_literals;
namespace sjsu::arm {
inline hal::degrees atan2_to_degrees(float y, float x)
{
  return atan2(y, x) * 180.0f / std::numbers::pi_v<float>;
}

inline void set_zero(hal::can::id_t p_addr, hal::can& p_can)
{
  hal::can::message_t set_zero;
  set_zero.id = p_addr;
  set_zero.payload = { 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  set_zero.length = 8;
  p_can.send(set_zero);
}

inline void reset_mc_x(hal::can::id_t p_addr, hal::can& p_can)
{
  hal::can::message_t reset;
  reset.id = p_addr;
  reset.payload = { 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  reset.length = 8;
  p_can.send(reset);
}

inline void home(hal::accelerometer& p_rotunda_accelerometer,
                 hal::accelerometer& p_shoulder_accelerometer,
                 hal::accelerometer& p_elbow_accelerometer,
                 [[maybe_unused]]hal::accelerometer& p_wrist_accelerometer,
                 [[maybe_unused]]hal::rmd::mc_x& p_rotunda_mc_x,
                 hal::rmd::mc_x& p_shoulder_mc_x,
                 hal::rmd::mc_x& p_elbow_mc_x,
                 [[maybe_unused]]hal::rmd::mc_x& p_left_wrist_mc_x,
                 [[maybe_unused]]hal::rmd::mc_x& p_right_wrist_mc_x,
                 hal::serial& p_terminal,
                 hal::can& p_can,
                 hal::steady_clock& p_clk)
{

  hal::degrees shoulder_error;
  hal::degrees elbow_error;
  hal::rpm elbow_speed;
  hal::rpm shoulder_speed;

  bool shoulder_homed = false, elbow_homed = false;
  while (!shoulder_homed || !elbow_homed) {
    auto rotunda_read = p_rotunda_accelerometer.read();
    hal::degrees rotunda_base_yz =
      atan2_to_degrees(rotunda_read.y, rotunda_read.z);  // elbow/shoulder 0

    auto shoulder_read = p_shoulder_accelerometer.read();
    shoulder_error =
      (rotunda_base_yz - 90) -
      atan2_to_degrees(shoulder_read.y, shoulder_read.z);  // shoulder error

    auto elbow_read = p_elbow_accelerometer.read();
    elbow_error =
      (atan2_to_degrees(shoulder_read.y, shoulder_read.z) + 90) -
      atan2_to_degrees(elbow_read.y, elbow_read.z);  // shoulder error

    shoulder_speed = shoulder_error * -1;
    shoulder_speed = std::clamp(shoulder_speed, -3.0f, 3.0f);

    elbow_speed = elbow_error * 1;
    elbow_speed = std::clamp(elbow_speed, -3.0f, 3.0f);

    if (abs(shoulder_error) < 1) {
      shoulder_homed = true;
    }
    if (abs(elbow_error) < 1) {
      elbow_homed = true;
    }

    if (!shoulder_homed) {
      p_shoulder_mc_x.velocity_control(shoulder_speed);
    } else {
      p_shoulder_mc_x.velocity_control(0);
    }

    if (!elbow_homed) {
      p_elbow_mc_x.velocity_control(elbow_speed);
    } else {
      p_elbow_mc_x.velocity_control(0);
    }
    hal::print<1024>(p_terminal, "%f \t %f\n", shoulder_speed, elbow_speed);
  }

  hal::print(p_terminal, "ZEROINGTHE MOTORS\n");
  set_zero(0x143, p_can);
  set_zero(0x142, p_can);
  hal::delay(p_clk, 1ms);

  hal::delay(p_clk, 1ms);
  while (true) {
    try {
      p_shoulder_mc_x.velocity_control(2);
      break;
    } catch (const hal::timed_out&) {
      continue;
    }
  }
  while (true) {
    try {
      p_elbow_mc_x.velocity_control(2);
      break;
    } catch (const hal::timed_out&) {
      continue;
    }
  }
  hal::delay(p_clk, 7s);
  while (true) {
    try {
      p_shoulder_mc_x.velocity_control(0);
      break;
    } catch (const hal::timed_out&) {
      continue;
    }
  }
  while (true) {
    try {
      p_elbow_mc_x.velocity_control(0);
      break;
    } catch (const hal::timed_out&) {
      continue;
    }
  }
  hal::delay(p_clk, 1ms);

  reset_mc_x(0x142, p_can);
  reset_mc_x(0x143, p_can);
  hal::delay(p_clk, 5s);

  hal::print(p_terminal, "MOVING TO 0 POSITION\n");
  constexpr int number_of_retries = 2;
  for (int i = 0; i < number_of_retries; i++) {
    p_shoulder_mc_x.position_control(0, 5);
    p_elbow_mc_x.position_control(0, 5);
    hal::delay(p_clk, 10ms);
  }
}
}  // namespace sjsu::arm