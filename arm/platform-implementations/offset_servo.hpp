#pragma once

#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>

namespace sjsu::arm {

class offset_servo : public hal::servo
{
public:
  offset_servo(hal::servo& p_servo, hal::degrees p_offset)
    : m_servo(&p_servo)
    , m_offset(p_offset)
  {
  }

  hal::degrees get_offset()
  {
    return m_offset;
  }

  void set_offset(hal::degrees p_offset)
  {
    m_offset = p_offset;
  }

private:
  void driver_position(hal::degrees p_position) override
  {
    m_servo->position(p_position + m_offset);
  }

  hal::servo* m_servo = nullptr;
  hal::degrees m_offset;
};

}  // namespace sjsu::arm
