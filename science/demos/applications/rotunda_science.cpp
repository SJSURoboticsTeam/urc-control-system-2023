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
        auto& magnet_my = *p_framework.magnet;


        while (true) 
        {
            hal::delay(clock,std::chrono::milliseconds(5));

            if (magnet_my.level().value().state)
            {
                hal::print<1024>(terminal,"2 slow 2 sad\n");
                HAL_CHECK(rotunda_science_my.position(hal::degrees(360.0)));
            }
            else 
            {
                hal::print<1024>(terminal,"2 fast 2 furious\n");
                HAL_CHECK(rotunda_science_my.position(hal::degrees(180.0)));
            }
        }
    } 
}