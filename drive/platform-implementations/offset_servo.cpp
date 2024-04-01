#include "../include/offset_servo.hpp"
#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>

namespace sjsu::drive {

offset_servo offset_servo::create(hal::servo& p_servo,
                                               hal::degrees p_offset)
{
  return offset_servo(p_servo, p_offset);
}
hal::degrees offset_servo::get_offset()
{
  return m_offset;
}

void offset_servo::set_offset(hal::degrees p_offset)
{
  m_offset = p_offset;
}
offset_servo::offset_servo(hal::servo& p_servo, hal::degrees p_offset)
  : m_servo(&p_servo)
  , m_offset(p_offset)
{
}

hal::servo::position_t offset_servo::driver_position(
  hal::degrees p_position)
{
  return m_servo->position(p_position + m_offset);
}

}  // namespace sjsu::drive
