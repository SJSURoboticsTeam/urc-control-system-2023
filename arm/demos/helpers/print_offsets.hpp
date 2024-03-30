#pragma once
#include "../../platform-implementations/offset_servo.hpp"
#include <libhal/serial.hpp>

namespace sjsu::arm {

inline void print_arm(offset_servo& rotunda,
                             offset_servo& shoulder,
                             offset_servo& elbow,
                             offset_servo& lwrist,
                             offset_servo& rwrist,
                             hal::serial& terminal)
{
  hal::print<1024>(terminal,
                   "Rotunda Offset: %f\n Shoulder Offset: %f \nElbow Offset: %f \n"
                   "Lwrist Offset: %f \nRwrist Offset: %f\n",
                   rotunda.get_offset(),
                   shoulder.get_offset(),
                   elbow.get_offset(),
                   lwrist.get_offset(),
                   rwrist.get_offset());
}
}  // namespace sjsu::arm