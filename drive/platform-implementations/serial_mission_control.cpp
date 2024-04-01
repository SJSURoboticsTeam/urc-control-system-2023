#include <libhal/serial.hpp>

#include <string_view>

#include "../include/serial_mission_control.hpp"

namespace sjsu::drive {

serial_mission_control::serial_mission_control(hal::serial& p_console)
  : m_console(p_console)
{
}

void serial_mission_control::parse_commands(std::string_view p_commands_json)
{

  static constexpr int expected_number_of_arguments = 6;
  mc_commands commands;

  int actual_arguments = sscanf(p_commands_json.data(),
                                kResponseBodyFormat,
                                &commands.heartbeat_count,
                                &commands.is_operational,
                                &commands.wheel_orientation,
                                &commands.mode,
                                &commands.speed,
                                &commands.angle);
  if (actual_arguments != expected_number_of_arguments) {
    hal::print<200>(m_console,
                    "Received %d arguments, expected %d\n",
                    actual_arguments,
                    expected_number_of_arguments);
  }
  m_commands = commands;
}

mission_control::mc_commands
serial_mission_control::impl_get_command(
  hal::function_ref<hal::timeout_function> p_timeout)
{
  std::array<hal::byte, 8192> buffer{};
  auto received = HAL_CHECK(m_console.read(buffer)).data;
  if (received.size() != 0) {
    auto response = std::string_view(
      reinterpret_cast<const char*>(received.data()), received.size());
    // hal::print<200>(
    //   m_console, "%.*s", static_cast<int>(response.size()), response.data());
    parse_commands(response);
  }
  return m_commands;
}
}  // namespace sjsu::drive