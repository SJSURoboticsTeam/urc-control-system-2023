#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal/input_pin.hpp>

#include "../hardware_map.hpp"

#include <string_view>
#include <array>
#include <cinttypes>

using namespace std::chrono_literals;
using namespace hal::literals;

namespace sjsu::science 
{
    hal::status application(sjsu::science::application_framework& p_framework) 
    {
        using namespace std::literals;

        auto& clock = *p_framework.clock; //
        auto& terminal = *p_framework.terminal;
        auto& rotunda_science_my = *p_framework.rotunda_science;
        auto& magnet_my = *p_framework.rotunda_magnet;

        HAL_CHECK(rotunda_science_my.position(hal::degrees(360.0)));

        while (true) 
        {
            hal::delay(clock,std::chrono::milliseconds(2000));

            if (!magnet_my.level())
            {
                hal::print<1024>(terminal,"2 slow 2 sad\n");
                HAL_CHECK(rotunda_science_my.position(hal::degrees(180.0)));
            }
            else 
            {
                hal::print<1024>(terminal, "2 fast 2 furious");
                HAL_CHECK(rotunda_science_my.position(hal::degrees(360.0)));
            }
        }
    }   
}