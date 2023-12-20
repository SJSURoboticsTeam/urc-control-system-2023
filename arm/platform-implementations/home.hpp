#include <libhal-rmd/mc_x.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/accelerometer.hpp>
#include <math.h>
#define M_PI 3.14156265

using namespace std::chrono_literals;
namespace sjsu::arm {
inline hal::degrees atan2_d(float y, float x)
{
  return atan2(y, x) * 180 / M_PI;
}


inline hal::status set_zero(hal::can::id_t p_addr, hal::can& p_can)
{
  hal::can::message_t set_zero;
  set_zero.id = p_addr;
  set_zero.payload = { 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  set_zero.length = 8;
  HAL_CHECK(p_can.send(set_zero));
  return hal::success();
}

inline hal::status reset_mc_x(hal::can::id_t p_addr, hal::can& p_can)
{
  hal::can::message_t reset;
  reset.id = p_addr;
  reset.payload = { 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  reset.length = 8;
  HAL_CHECK(p_can.send(reset));
  return hal::success();
}

inline hal::status home(hal::accelerometer& p_rotunda_accelerometer,
                        hal::accelerometer& p_shoulder_accelerometer,
                        hal::accelerometer& p_elbow_accelerometer,
                        hal::accelerometer& p_wrist_accelerometer,
                        hal::rmd::mc_x& p_rotunda_mc_x,
                        hal::rmd::mc_x& p_shoulder_mc_x,
                        hal::rmd::mc_x& p_elbow_mc_x,
                        hal::rmd::mc_x& p_left_wrist_mc_x,
                        hal::rmd::mc_x& p_right_wrist_mc_x,
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
    auto rotunda_read = HAL_CHECK(p_rotunda_accelerometer.read());
    hal::degrees rotunda_base_yz =
      atan2_d(rotunda_read.y, rotunda_read.z);  // elbow/shoulder 0

    auto shoulder_read = HAL_CHECK(p_shoulder_accelerometer.read());
    shoulder_error =
      (rotunda_base_yz - 90) -
      atan2_d(shoulder_read.y, shoulder_read.z);  // shoulder error

    auto elbow_read = HAL_CHECK(p_elbow_accelerometer.read());
    elbow_error = (atan2_d(shoulder_read.y, shoulder_read.z) + 90) -
                  atan2_d(elbow_read.y, elbow_read.z);  // shoulder error

    hal::print<1024>(
      p_terminal, "\nERROR: %f \t %f\n", shoulder_error, elbow_error);
    hal::delay(p_clk, 100ms);

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
      HAL_CHECK(p_shoulder_mc_x.velocity_control(shoulder_speed));
    } else {
      HAL_CHECK(p_shoulder_mc_x.velocity_control(0));
    }

    if (!elbow_homed) {
      HAL_CHECK(p_elbow_mc_x.velocity_control(elbow_speed));
    } else {
      HAL_CHECK(p_elbow_mc_x.velocity_control(0));
    }
    hal::print<1024>(p_terminal, "%f \t %f\n", shoulder_speed, elbow_speed);
  }

  HAL_CHECK(p_shoulder_mc_x.velocity_control(0));
  hal::delay(p_clk, 10ms);
  HAL_CHECK(p_elbow_mc_x.velocity_control(0));
  hal::delay(p_clk, 10ms);

  HAL_CHECK(set_zero(0x142, p_can));
  hal::delay(p_clk, 10ms);
  HAL_CHECK(reset_mc_x(0x142, p_can));
  hal::delay(p_clk, 10ms);

  HAL_CHECK(reset_mc_x(0x143, p_can));
  hal::delay(p_clk, 10ms);
  HAL_CHECK(set_zero(0x143, p_can));
  hal::delay(p_clk, 10ms);

  HAL_CHECK(p_shoulder_mc_x.velocity_control(0));
  hal::delay(p_clk, 10ms);
  HAL_CHECK(p_elbow_mc_x.velocity_control(0));
  hal::delay(p_clk, 10ms);

  return hal::success();
}
}  // namespace sjsu::arm