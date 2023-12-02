#pragma once
#include <libhal/error.hpp>
#include <libhal-util/serial.hpp>
#include <cstdio>

namespace arm_mimic{

   hal::status send_data_to_mc(hal::serial &uart, std::array<float, 6> raw_data, bool is_network = false)
    {
        const char *json_format_str = "{\"heartbeat_count\":0,\"is_operational\":1,\"speed\":2,\"angles\":[%d,%d,%d,%d,%d,%d]}\n";
        char buffer[200];
        snprintf(buffer, 200, json_format_str,
                 static_cast<int>(raw_data[0]), static_cast<int>(raw_data[1]), static_cast<int>(raw_data[2]), static_cast<int>(raw_data[3]), static_cast<int>(raw_data[4]), static_cast<int>(raw_data[5]));
        if (!is_network)
            hal::print<200>(uart, buffer);
        else
            return hal::new_error("Not implemented");

        return hal::success();
    }

} // namespace arm_mimic
        // Structure of json file
        /*
        {
            heartbeat_count: 0,
            is_operational: 1,
            speed: 2,
            angles: [$ROTUNDA, $SHOULDER, $ELBOW, $WRIST_PITCH, $WRIST_ROLL, $END_EFFECTOR]
        }
        */