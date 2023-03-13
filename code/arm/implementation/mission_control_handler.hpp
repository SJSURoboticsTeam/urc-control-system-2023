#pragma once
#include <libhal-util/serial.hpp>

#include "../dto/arm_dto.hpp"
#include "../dto/feedback_dto.hpp"

namespace arm {
inline hal::result<mc_commands> parse_mission_control_data(
  std::string response,
  hal::serial& terminal)
{
  static constexpr int expected_number_of_arguments = 9;
  mc_commands commands;
  response = response.substr(response.find('{'));
  int actual_arguments = sscanf(response.c_str(),
                                kResponseBodyFormat,
                                &commands.heartbeat_count,
                                &commands.is_operational,
                                &commands.speed,
                                &commands.rotunda_angle,
                                &commands.shoulder_angle,
                                &commands.elbow_angle,
                                &commands.wrist_pitch_angle,
                                &commands.wrist_roll_angle,
                                &commands.rr9_angle);
  if (actual_arguments != expected_number_of_arguments) {
    HAL_CHECK(hal::write(
      terminal,
      std::string("Received " + actual_arguments) +
        std::string("arguments, expected " + expected_number_of_arguments)));
    return hal::new_error(std::errc::invalid_argument);
  }
  return commands;
}
}  // namespace arm
