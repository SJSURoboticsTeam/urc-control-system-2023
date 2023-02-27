#pragma once
#include <array>
#include <libhal-util/serial.hpp>

#include <string_view>

#include "../dto/science_dto.hpp"

namespace science
{
    class MissionControlHandler
    {
    public:
        std::string_view CreateGETRequestParameterWithRoverStatus(science_data data)
        {
            char request_parameter[200];
            snprintf(
                request_parameter, 200, kGETRequestFormat,
                data.status.move_revolver_status, data.status.seal_status, data.status.depressurize_status,
                data.status.inject_status, data.status.clear_status, data.status.unseal_status, 
                static_cast<int>(data.methane_level), static_cast<int>(data.co2_level), static_cast<int>(data.pressure_level)
                );
            return request_parameter;
        }

        hal::result<science_commands> ParseMissionControlData(std::string& response, hal::serial& terminal)
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