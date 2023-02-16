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
  hal::degrees rotunda_angle = hal::degrees(0);
  hal::degrees shoulder_angle = hal::degrees(0);
  hal::degrees elbow_angle = hal::degrees(0);
  hal::degrees wrist_angle = hal::degrees(0);

  void Print()
  {
  }
};
}  // namespace Arm