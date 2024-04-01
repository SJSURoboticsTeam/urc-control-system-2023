#include <libhal-lpc40/input_pin.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>

#include "../applications/application.hpp"
#include "../dto/drive.hpp"
#include "../dto/motor_feedback.hpp"
#include "../include/tri_wheel_router.hpp"

namespace sjsu::drive {

tri_wheel_router::tri_wheel_router(leg& p_back, leg& p_right, leg& p_left)
  : m_left(p_left)
  , m_back(p_back)
  , m_right(p_right)
{
}

void tri_wheel_router::move(
  tri_wheel_router_arguments p_tri_wheel_arguments)
{
  m_left.steer->position(-p_tri_wheel_arguments.left.angle);
  m_left.propulsion->power(p_tri_wheel_arguments.left.speed / 100);

  m_right.steer->position(-p_tri_wheel_arguments.right.angle);
  
  m_right.propulsion->power(-p_tri_wheel_arguments.right.speed / 100);

  m_back.steer->position(-p_tri_wheel_arguments.back.angle);
  m_back.propulsion->power(p_tri_wheel_arguments.back.speed / 100);

}

motor_feedback tri_wheel_router::get_motor_feedback()
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  motor_feedback motor_speeds;

  motor_speeds.left_drive_speed = m_left.propulsion_speed_sensor->read().speed;
  motor_speeds.left_steer_speed = m_left.steer_speed_sensor->read().speed;
  motor_speeds.back_drive_speed = m_back.propulsion_speed_sensor->read().speed;
  motor_speeds.back_steer_speed = m_back.steer_speed_sensor->read().speed;
  motor_speeds.right_drive_speed = m_right.propulsion_speed_sensor->read().speed;
  motor_speeds.right_steer_speed = m_right.steer_speed_sensor->read().speed;

  return motor_speeds;
}

}  // namespace sjsu::drive