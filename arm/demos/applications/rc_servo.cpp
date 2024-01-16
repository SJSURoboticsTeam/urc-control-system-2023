#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include "../hardware_map.hpp"
#include <string_view>
#include <array>
#include <cinttypes>
using namespace std::chrono_literals;
using namespace hal::literals;

namespace sjsu::arm {
std::string_view to_string_view(std::span<const hal::byte> p_span)
{
return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                        p_span.size());
}
hal::status application(sjsu::arm::application_framework& p_framework) {
    // auto& end_effector_my = *p_framework.end_effector;
    using namespace std::literals;

    auto& clock = *p_framework.clock;
    auto& terminal = *p_framework.terminal;
    std::array<hal::byte, 1024> received_buffer{};
    while (true) {
        hal::delay(clock, 3000ms);
        
        auto readings = HAL_CHECK(terminal.read(received_buffer));
        hal::print<200>(terminal, "readings: %s\n", readings.data);
        auto result = to_string_view(readings.data);
        // result = result.substr(result.find('{'));
        if (result.find("{") == std::string_view::npos){
            hal::print(terminal, "could not find\n");
        }
        else{
            result = result.substr(result.find('{') +1 );
            // char * end; //string view is not null terminated

            float location = (std::clamp(std::stof(result.data()),0.0f,100.0f)/100) * 110; //clamps to value
            hal::print<200>(terminal, "Go to: %f\n", location);
        }

        // HAL_CHECK(end_effector_my.position(110.0_deg));

        // hal::delay(clock, 3000ms);
        // HAL_CHECK(end_effector_my.position(0.0_deg));
        // int actual_arguments = sscanf(result.data(),
        //                                 kResponseBodyFormat,
        //                                 &commands.heartbeat_count,
        //                                 &commands.is_operational,
        //                                 &commands.speed,
        //                                 &commands.rotunda_angle,
        //                                 &commands.shoulder_angle,
        //                                 &commands.elbow_angle,
        //                                 &commands.wrist_pitch_angle,
        //                                 &commands.wrist_roll_angle,
        //                                 &commands.rr9_angle);
        
    }


    return hal::success();
}


}