#pragma once

#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>

namespace sjsu::drive {

class offset_servo : public hal::servo
{
public:
  // static offset_servo create(hal::servo& p_servo,
  //                                         hal::degrees p_offset);

  hal::degrees get_offset();

  void set_offset(hal::degrees p_offset);

  offset_servo(hal::servo& p_servo, hal::degrees p_offset);


private:

  void driver_position(
    hal::degrees p_position) override;
    
  hal::servo* m_servo = nullptr;
  hal::degrees m_offset;
};

}  // namespace sjsu::drive
