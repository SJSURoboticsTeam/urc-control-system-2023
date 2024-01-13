#include "../platform-implementations/offset_servo.hpp"
#include <libhal/accelerometer.hpp>
#include <math.h>
#define M_PI 3.14156265

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
                        offset_servo& p_right_wrist_servo)
{

  auto rotunda_read = HAL_CHECK(p_rotunda_accelerometer.read());
  auto shoulder_read = HAL_CHECK(p_shoulder_accelerometer.read());
  auto elbow_read = HAL_CHECK(p_elbow_accelerometer.read());
  auto wrist_read = HAL_CHECK(p_wrist_accelerometer.read());

  hal::degrees rotunda_base_yz =
    atan2_d(rotunda_read.y, rotunda_read.z);  // elbow/shoulder 0
  hal::degrees rotunda_base_yx =
    atan2_d(rotunda_read.y, rotunda_read.x);  // wr roll 0

  hal::degrees shoulder_error =
    90 + rotunda_base_yz -
    atan2_d(shoulder_read.y, shoulder_read.z);  // shoulder error
  hal::degrees elbow_error =
    rotunda_base_yz - atan2_d(elbow_read.y, elbow_read.z);  // elbow error

  hal::degrees wrist_roll_error =
    rotunda_base_yx - atan2_d(wrist_read.y, wrist_read.x);
  hal::degrees wrist_pitch_error =
    rotunda_base_yz - atan2_d(wrist_read.y, wrist_read.z);

  p_shoulder_servo.set_offset(shoulder_error + p_shoulder_servo.get_offset());
  p_elbow_servo.set_offset(elbow_error + p_elbow_servo.get_offset() + p_shoulder_servo.get_offset());
  p_left_wrist_servo.set_offset(static_cast<float>(wrist_roll_error) +
                                static_cast<float>(wrist_pitch_error) + p_left_wrist_servo.get_offset());
  p_right_wrist_servo.set_offset(static_cast<float>(wrist_roll_error) -
                                 static_cast<float>(wrist_pitch_error) + p_right_wrist_servo.get_offset());

  return hal::success();
}
}  // namespace sjsu::arm