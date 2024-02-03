#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../hardware_map.hpp"

#include <string_view>
#include <array>
#include <cinttypes>

using namespace std::chrono_literals;
using namespace hal::literals;

namespace sjsu::science 
{
    std::string_view to_string_view(std::span<const hal::byte> p_span)
    {
        return std::string_view(reinterpret_cast<const char*>(p_span.data()),
            p_span.size());
    }

    hal::status application(sjsu::science::application_framework& p_framework) 
    {
        using namespace std::literals;

        auto& clock = *p_framework.clock; //
        auto& terminal = *p_framework.terminal;
        auto& rotunda_science_my = *p_framework.rotunda_science;

        std::array<hal::byte, 1024> received_buffer{};

        while (true) 
        {
            hal::delay(clock,std::chrono::milliseconds(2000));
            auto readings = HAL_CHECK(terminal.read(received_buffer));
            hal::print<1024>(terminal, "readings: %s\n", readings.data);
            auto result = to_string_view(received_buffer);

            if (result.find("{") == std::string_view::npos)
            {
                hal::print<1024>(terminal,"2 slow 2 sad\n");
                HAL_CHECK(rotunda_science_my.position(hal::degrees(360.0)));
            }
            else 
            {
                result = result.substr(result.find('{') +1 );
                float location = (std::clamp(std::stof(result.data()),0.0f,360.0f)); //clamps to value
                hal::print<1024>(terminal, "2 fast 2 furious @ %f degrees\n", location);
                HAL_CHECK(rotunda_science_my.position(static_cast<float>(location)));
            }
        }
    }   
}