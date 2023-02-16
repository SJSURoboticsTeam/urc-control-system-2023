#pragma once
#include <array>
#include <libhal-util/serial.hpp>

#include <string_view>

#include "../dto/science-dto.hpp"

namespace Science
{
    class MissionControlHandler
    {
    public:
        std::string_view CreateGETRequestParameterWithRoverStatus(science_commands commands)
        {
            char request_parameter[200];
            // snprintf(
            //     request_parameter, 200, kGETRequestFormat,
            //     commands.heartbeat_count, commands.is_operational, commands.wheel_orientation, commands.mode, commands.speed, commands.angle);
            return request_parameter;
        }

        hal::result<science_commands> ParseMissionControlData(std::string_view& response, hal::serial& terminal)
        {
            response = response.substr(response.find('{'));
            int actual_arguments = 6;                           // This will need to be changed when we are ready to implement mission control for science
            // int actual_arguments = sscanf(
            //     response.c_str(), kResponseBodyFormat,
            //     &commands_.heartbeat_count, &commands_.is_operational, &commands_.wheel_orientation,
            //     &commands_.mode, &commands_.speed, &commands_.angle);
            if (actual_arguments != kExpectedNumberOfArguments)
            {
                HAL_CHECK(hal::write(terminal, std::string("Received " + actual_arguments) + std::string("arguments, expected " + kExpectedNumberOfArguments)));
            }
            return commands_;
        }

    private:
        drive_commands commands_;
        const int kExpectedNumberOfArguments = 6;
    };

}