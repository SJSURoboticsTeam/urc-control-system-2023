#pragma once
#include <array>
#include <libhal-util/serial.hpp>

#include <string_view>

#include "../dto/science-dto.hpp"

namespace science
{
    class MissionControlHandler
    {
    public:
        std::string_view CreateGETRequestParameterWithRoverStatus(science_data commands)
        {
            char request_parameter[200];
            snprintf(
                request_parameter, 200, kGETRequestFormat,
                commands_.move_revolver_status, commands_.seal_status, commands_.depressurize_status,
                commands_.inject_status, commands_.clear_status, commands_.unseal_status, 
                commands_.methane_level, commands_.co2_level, commands_.pressure_level
                );
            return request_parameter;
        }

        hal::result<science_commands> ParseMissionControlData(std::string_view& response, hal::serial& terminal)
        {
            response = response.substr(response.find('{'));
            int actual_arguments = sscanf(
                response.c_str(), kResponseBodyFormat,
                &commands_.state_step, &commands_.mode, &commands_.is_operational);
            if (actual_arguments != kExpectedNumberOfArguments)
            {
                HAL_CHECK(hal::write(terminal, std::string("Received " + actual_arguments) + std::string("arguments, expected " + kExpectedNumberOfArguments)));
            }
            return commands_;
        }

    private:
        science_commands commands_;
        const int kExpectedNumberOfArguments = 3;
    };

}