#pragma once

#include <libhal-lpc40/input_pin.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal/servo.hpp>

#include "../applications/application.hpp"
#include "../dto/drive.hpp"
#include "../dto/motor_feedback.hpp"

namespace sjsu::drive {
class tri_wheel_router
{
public:
  tri_wheel_router(leg& p_back, leg& p_right, leg& p_left);

  void move(tri_wheel_router_arguments p_tri_wheel_arguments);

  motor_feedback get_motor_feedback();

private:
  // member variables

  leg& m_left;
  leg& m_back;
  leg& m_right;
};
}  // namespace sjsu::drive
