
#include "../include/command_lerper.hpp"
#include "../dto/drive.hpp"

namespace sjsu::drive {

int command_lerper::lerp(float p_speed)
{
  m_previous_speed =
    static_cast<int>(std::lerp(m_previous_speed, p_speed, m_lerp_speed));
  return m_previous_speed;
}
}  // namespace sjsu::drive
