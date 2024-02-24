#pragma once

#include "../applications/application.hpp"

namespace sjsu::drive {
/**
 * @brief A class that can represent any configuration of wheels.
 * 
 */
class wheel_router
{
public:
  /**
   * @brief Create a wheel router
   * 
   * @param m_legs 
   */
  wheel_router(std::span<leg*> m_legs);
  
  /**
   * @brief Move each wheel to a given setting.
   * @warning If `p_settings.size() != m_legs.size()` the function immediately returns without returning an error. This should be fixed
   * 
   * @param p_settings 
   * @return hal::status 
   */
  hal::status move(std::span<wheel_setting> p_settings);

//   hal::result<motor_feedback> get_motor_feedback();

private:
  // member variables

  std::span<leg*> m_legs;
};
}  // namespace sjsu::drive
