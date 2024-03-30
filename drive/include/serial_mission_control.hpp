#pragma once

#include <libhal/serial.hpp>

#include <string_view>

#include "mission_control.hpp"

namespace sjsu::drive {

class serial_mission_control : public mission_control
{
public:
  [[nodiscard]] static hal::result<serial_mission_control> create(
    hal::serial& p_console)
  {
    serial_mission_control s_mission_control(p_console);

    return s_mission_control;
  }

private:
  serial_mission_control(hal::serial& p_console);

  void parse_commands(std::string_view p_commands_json);

  mission_control::mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override;

  hal::serial& m_console;
  mc_commands m_commands;
};
}  // namespace sjsu::drive