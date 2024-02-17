#include <libhal-rmd/mc_x.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/accelerometer.hpp>
#include <math.h>

using namespace std::chrono_literals;
namespace sjsu::arm {
inline hal::degrees atan2_to_degrees(float y, float x)
{
  return atan2(y, x) * 180.0f / std::numbers::pi_v<float>;
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
      atan2_to_degrees(rotunda_read.y, rotunda_read.z);  // elbow/shoulder 0

    auto shoulder_read = HAL_CHECK(p_shoulder_accelerometer.read());
    shoulder_error =
      (rotunda_base_yz - 90) -
      atan2_to_degrees(shoulder_read.y, shoulder_read.z);  // shoulder error

    auto elbow_read = HAL_CHECK(p_elbow_accelerometer.read());
    elbow_error = (atan2_to_degrees(shoulder_read.y, shoulder_read.z) + 90) -
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

    hal::print<1024>(p_terminal, "%f \t %f\n", shoulder_speed, elbow_speed);
    if (!shoulder_homed) {
      while(!p_shoulder_mc_x.velocity_control(shoulder_speed));
    } else {
      while(!p_shoulder_mc_x.velocity_control(0));
    }

    if (!elbow_homed) {
      while(!p_elbow_mc_x.velocity_control(elbow_speed));
    } else {
      while(!p_elbow_mc_x.velocity_control(0));
    }
  }

  hal::print(p_terminal, "ZEROINGTHE MOTORS\n");

  HAL_CHECK(set_zero(0x145, p_can));
  HAL_CHECK(set_zero(0x144, p_can));
  HAL_CHECK(set_zero(0x143, p_can));
  HAL_CHECK(set_zero(0x142, p_can));
  HAL_CHECK(set_zero(0x141, p_can));
  
  hal::delay(p_clk, 1ms);

  hal::delay(p_clk, 1ms);
  while(!p_shoulder_mc_x.velocity_control(2)){
    continue;
  }
  while(!p_elbow_mc_x.velocity_control(2)){
    continue;
  }
  hal::delay(p_clk, 7s);
  while(!p_shoulder_mc_x.velocity_control(0)){
    continue;
  }
  while(!p_elbow_mc_x.velocity_control(0)){
    continue;
  }
  hal::delay(p_clk, 1ms);


  HAL_CHECK(reset_mc_x(0x145, p_can));
  HAL_CHECK(reset_mc_x(0x144, p_can));
  HAL_CHECK(reset_mc_x(0x143, p_can));
  HAL_CHECK(reset_mc_x(0x142, p_can));
  HAL_CHECK(reset_mc_x(0x141, p_can));
  hal::delay(p_clk, 5s);

  hal::print(p_terminal, "MOVING TO 0 POSITION\n");
  constexpr int  number_of_retries = 2;
  for(int i = 0; i < number_of_retries; i ++){
    HAL_CHECK(p_left_wrist_mc_x.position_control(0, 5));
    HAL_CHECK(p_right_wrist_mc_x.position_control(0, 5));
    HAL_CHECK(p_shoulder_mc_x.position_control(0, 5));
    HAL_CHECK(p_elbow_mc_x.position_control(0, 5));
    hal::delay(p_clk, 10ms);
  }

  return hal::success();
  }
}  // namespace sjsu::arm