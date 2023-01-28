#pragma once
#include <libhal-util/units.hpp>

namespace Arm {
struct accelerometer_feedback
{
  int x = 0;
  int y = 0;
  int z = 0;
};

struct arm_accelerometer_feedback
{
  accelerometer_feedback rotunda{};
  accelerometer_feedback shoulder{};
  accelerometer_feedback elbow{};
  accelerometer_feedback wrist{};

  void Print()
  {
  }
};

struct motors_feedback
{
  hal::rpm rotunda_speed = hal::rpm(0);
  hal::rpm shoulder_speed = hal::rpm(0);
  hal::rpm elbow_speed = hal::rpm(0);
  hal::rpm wrist_speed = hal::rpm(0);
  hal::angle rotunda_angle = hal::angle(0);
  hal::angle shoulder_angle = hal::angle(0);
  hal::angle elbow_angle = hal::angle(0);
  hal::angle wrist_angle = hal::angle(0);

  void Print()
  {
  }
};
}  // namespace Arm