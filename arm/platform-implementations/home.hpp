#include "../platform-implementations/offset_servo.hpp"
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
inline hal::status home(hal::accelerometer& p_rotunda_accelerometer,
                        hal::accelerometer& p_shoulder_accelerometer,
                        hal::accelerometer& p_elbow_accelerometer,
                        hal::accelerometer& p_wrist_accelerometer,
                        offset_servo& p_rotunda_servo,
                        offset_servo& p_shoulder_servo,
                        offset_servo& p_elbow_servo,
                        offset_servo& p_left_wrist_servo,
                        offset_servo& p_right_wrist_servo,
                        hal::serial& terminal,
                        hal::steady_clock& clk)
{
  auto rotunda_read = HAL_CHECK(p_rotunda_accelerometer.read());
  auto elbow_read = HAL_CHECK(p_elbow_accelerometer.read());
  auto wrist_read = HAL_CHECK(p_wrist_accelerometer.read());

  hal::degrees rotunda_base_yz =
    atan2_d(rotunda_read.y, rotunda_read.z);  // elbow/shoulder 0
  hal::degrees rotunda_base_yx =
    atan2_d(rotunda_read.y, rotunda_read.x);  // wr roll 0

  hal::degrees shoulder_error;

  hal::print(terminal, "HOMING SHOULDER \n");
  hal::print<1024>(terminal, "shoulder: off: %f\n", p_shoulder_servo.get_offset());

  do{
    auto shoulder_read = HAL_CHECK(p_shoulder_accelerometer.read());

    shoulder_error =
      (rotunda_base_yz - 90) -
      atan2_d(shoulder_read.y, shoulder_read.z);  // shoulder error
    hal::print<1024>(terminal, "\nERROR: %f\n", shoulder_error);

    p_shoulder_servo.set_offset(
      (p_shoulder_servo.get_offset() - shoulder_error));

    hal::print<1024>(terminal, "shoulder: off: %f\n", p_shoulder_servo.get_offset());
    HAL_CHECK(p_shoulder_servo.position(0));
    hal::delay(clk, 15s);

    shoulder_read = HAL_CHECK(p_shoulder_accelerometer.read());
    shoulder_error =
      (rotunda_base_yz - 90) -
      atan2_d(shoulder_read.y, shoulder_read.z); 
    hal::print<1024>(terminal, "VALIDATION ERROR: %f\n", shoulder_error);
    
  }while(abs(shoulder_error) > 5); 

  hal::degrees elbow_error =
    rotunda_base_yz - atan2_d(elbow_read.y, elbow_read.z);  // elbow error

  hal::degrees wrist_roll_error =
    rotunda_base_yx - atan2_d(wrist_read.y, wrist_read.x);
  hal::degrees wrist_pitch_error =
    rotunda_base_yz - atan2_d(wrist_read.y, wrist_read.z);

  hal::print<1024>(terminal,
                   "shoulder_error: %f\n elbow_error: %f\n wrr_error: %f\n "
                   "wrp_error: %f\n",
                   shoulder_error,
                   elbow_error,
                   wrist_roll_error,
                   wrist_pitch_error);

  p_elbow_servo.set_offset(elbow_error + p_elbow_servo.get_offset() +
                           p_shoulder_servo.get_offset());
  p_left_wrist_servo.set_offset(static_cast<float>(wrist_roll_error) +
                                static_cast<float>(wrist_pitch_error) +
                                p_left_wrist_servo.get_offset());
  p_right_wrist_servo.set_offset(static_cast<float>(wrist_roll_error) -
                                 static_cast<float>(wrist_pitch_error) +
                                 p_right_wrist_servo.get_offset());

  return hal::success();
}
}  // namespace sjsu::arm