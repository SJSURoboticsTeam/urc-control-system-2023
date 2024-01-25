#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>

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
    using namespace std::literals;
    auto& terminal = *p_framework.terminal;

    auto& end_effector_my = *p_framework.end_effector;
   
    auto& clock = *p_framework.clock;
    std::array<hal::byte, 1024> received_buffer{}; //{100
    while (true) {

        hal::delay(clock, 2000ms);


        auto readings = HAL_CHECK(terminal.read(received_buffer)); //.data,
        // hal::print(terminal, "something happened\n");

        hal::print<200>(terminal, "readings: %s\n", readings.data);
        auto result = to_string_view(received_buffer);
        if (result.find("{") == std::string_view::npos){
            hal::print(terminal, "could not find\n");
        }
        else{
            hal::print(terminal, "here\n");

            result = result.substr(result.find('{') +1 );
            // char * end; //string view is not null terminated
            float location = (std::clamp(std::stof(result.data()),0.0f,100.0f)/100) * 110; //clamps to value
            hal::print<200>(terminal, "Go to: %f\n", location);
            // hal::delay(clock, 10ms);
            HAL_CHECK(end_effector_my.position(static_cast<float>(location)));
        }

    }


    return hal::success();
}


}