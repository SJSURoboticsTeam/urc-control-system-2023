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

  tri_wheel_router(leg& p_back, leg& p_right, leg& p_left);

  hal::status move(tri_wheel_router_arguments p_tri_wheel_arguments,
                   hal::steady_clock& p_clock);

  hal::result<motor_feedback> get_motor_feedback();

private:

  // member variables

  leg& m_left;
  leg& m_back;
  leg& m_right;
};
}  // namespace Drive
