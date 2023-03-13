#pragma once
#include <array>
#include <libhal-util/serial.hpp>
#include <string>

#include <string_view>

#include "../dto/drive-dto.hpp"

namespace Drive {
// std::string CreateGETRequestParameterWithRoverStatus(drive_commands commands)
// {
//   char request_parameter[200];
//   snprintf(request_parameter,
//            200,
//            kGETRequestFormat,
//            commands.heartbeat_count,
//            commands.is_operational,
//            commands.wheel_orientation,
//            commands.mode,
//            commands.speed,
//            commands.angle);
//   return request_parameter;
// }

inline hal::result<drive_commands> parse_mission_control_data(
  std::string_view response,
  hal::serial& terminal)
{
  static constexpr int expected_number_of_arguments = 6;
  drive_commands commands;
  response = response.substr(response.find('{'));
  int actual_arguments = sscanf(response.data(),
                                kResponseBodyFormat,
                                &commands.heartbeat_count,
                                &commands.is_operational,
                                &commands.wheel_orientation,
                                &commands.mode,
                                &commands.speed,
                                &commands.angle);
  if (actual_arguments != expected_number_of_arguments) {
    hal::print<200>(terminal,
                    "Received %d arguments, expected %d\n",
                    actual_arguments,
                    expected_number_of_arguments);
    return hal::new_error(std::errc::invalid_argument);
  }
  return commands;
}

}  // namespace Drive
