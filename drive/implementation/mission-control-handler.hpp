#pragma once
#include <array>
#include <libhal-util/serial.hpp>
#include <string>

#include <string_view>

#include "../dto/drive-dto.hpp"

namespace Drive {
class MissionControlHandler
{
public:
  std::string CreateGETRequestParameterWithRoverStatus(drive_commands commands)
  {
    char request_parameter[200];
    snprintf(request_parameter,
             200,
             kGETRequestFormat,
             commands.heartbeat_count,
             commands.is_operational,
             commands.wheel_orientation,
             commands.mode,
             commands.speed,
             commands.angle);
    return request_parameter;
  }

  hal::result<drive_commands> ParseMissionControlData(std::string& response,
                                                      hal::serial& terminal)
  {
    response = response.substr(response.find('{'));
    int actual_arguments = sscanf(response.c_str(),
                                  kResponseBodyFormat,
                                  &commands_.heartbeat_count,
                                  &commands_.is_operational,
                                  &commands_.wheel_orientation,
                                  &commands_.mode,
                                  &commands_.speed,
                                  &commands_.angle);
    if (actual_arguments != kExpectedNumberOfArguments) {
      hal::print<200>(terminal,
                      "Received %d arguments, expected %d\n",
                      actual_arguments,
                      kExpectedNumberOfArguments);
    }
    return commands_;
  }

private:
  drive_commands commands_;
  const int kExpectedNumberOfArguments = 6;
};

}  // namespace Drive