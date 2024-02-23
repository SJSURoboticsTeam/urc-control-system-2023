#pragma once

#include "../applications/application.hpp"

namespace sjsu::drive {
class wheel_router
{
public:
  wheel_router(std::span<leg*> m_legs);

  hal::status move(std::span<wheel_setting> p_settings);

//   hal::result<motor_feedback> get_motor_feedback();

private:
  // member variables

  std::span<leg*> m_legs;
};
}  // namespace sjsu::drive
