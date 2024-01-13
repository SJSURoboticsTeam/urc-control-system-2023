#pragma once

#include "../dto/drive.hpp"

namespace sjsu::drive {
class command_lerper
{
public:
  int lerp(float p_speed);

private:
  static constexpr float m_lerp_speed = 0.1f;
  float m_previous_speed = 0;
};
}  // namespace sjsu::drive
