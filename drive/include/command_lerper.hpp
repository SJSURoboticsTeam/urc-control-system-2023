#pragma once

#include "../dto/drive.hpp"
#include "settings.hpp"

namespace sjsu::drive {
class command_lerper
{
public:
  int lerp(float p_speed);

private:
  static constexpr float m_lerp_speed = settings::speed_lerp_speed;
  float m_previous_speed = 0;
};
}  // namespace sjsu::drive
