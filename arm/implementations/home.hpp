#include "../platform-implementations/offset_servo.hpp"
#include <libhal/accelerometer.hpp>
#include <math.h>

namespace sjsu::arm {
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

  hal::degrees rotunda_base_xy = atan2(rotunda_read.y, rotunda_read.x);  // elbow/shoulder 0
  hal::degrees rotunda_base_yz = atan2(rotunda_read.z, rotunda_read.y);  // wr roll 0

  hal::degrees shoulder_error =
    90 + rotunda_base_xy -
    atan2(shoulder_read.y, shoulder_read.x);  // shoulder error
  hal::degrees elbow_error =
    rotunda_base_xy - atan2(elbow_read.y, elbow_read.x);  // shoulder error

  hal::degrees wrist_roll_error;
  hal::degrees wrist_pitch_error;

  p_elbow_servo.set_offset(elbow_error);

  p_left_wrist_servo.set_offset(
    static_cast<float>(wrist_roll_error) +
    static_cast<float>(wrist_pitch_error));

  p_right_wrist_servo.set_offset(
    static_cast<float>(wrist_roll_error) -
    static_cast<float>(wrist_pitch_error));

  return hal::success();
}
}  // namespace sjsu::arm