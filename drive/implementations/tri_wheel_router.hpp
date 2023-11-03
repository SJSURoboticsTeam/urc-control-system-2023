#pragma once

#include <libhal-lpc40/input_pin.hpp>
#include <libhal/servo.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>

#include "../dto/drive.hpp"
#include "../dto/motor_feedback.hpp"
#include "../applications/application.hpp"

namespace sjsu::drive {
class tri_wheel_router
{
public:

  tri_wheel_router(std::span<leg&> p_legs)
  : m_legs(p_legs)
  {
  }

  hal::status move(tri_wheel_router_arguments p_tri_wheel_arguments,
                   hal::steady_clock& p_clock)
  {
    for(int i = 0; i < m_legs.size(); i++) {
      HAL_CHECK(m_legs[i].steer->position(-p_tri_wheel_arguments.args[i].angle));
      HAL_CHECK(m_left.propulsion->power(p_tri_wheel_arguments.args[i].speed/100));      
    }

    return hal::success();
  }

  hal::result<motor_feedback> get_motor_feedback()
  {
    using namespace std::chrono_literals;
    using namespace hal::literals;
    motor_feedback motor_speeds;
    
    for(int i = 0; i < m_legs.size(); i++) {
          motor_speeds[i].drive = HAL_CHECK(m_legs[i].propulsion_speed_sensor->read()).speed;
          motor_speeds[i].speed = HAL_CHECK(m_legs[i].steer_speed_sensor->read()).speed;
    }

    return motor_speeds;
  }

private:

  // member variables
  std::span<leg&> m_legs;
};
}  // namespace Drive
