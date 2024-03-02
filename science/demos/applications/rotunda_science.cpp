#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../hardware_map.hpp"

#include <string>
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

        hal::degrees m_clockwise = hal::degrees(0.0);
        hal::degrees m_counterclockwise = hal::degrees(180.0);
        hal::degrees m_stop = hal::degrees(360.0);

        int count = -1; // account for initial state change
        bool hallState;
        bool hallStateDelayed;

        std::array<hal::byte, 1024> received_buffer{};

        while (true)
        {
            auto readings = HAL_CHECK(terminal.read(received_buffer));
            auto result = to_string_view(received_buffer);	

            if (result.find("{") == std::string_view::npos)
            {
                hal::print<1024>(terminal,"Awaiting User Input: \n");
            }
            else
            {
                result = result.substr(result.find('{') + 1 );

                // int vial = (std::clamp(std::stoi(result.data()),-m_numVials,m_numVials));
                int vial = 6;
                std::chrono::milliseconds m_delay = std::chrono::milliseconds(std::stoi(result.data()));

                HAL_CHECK(rotunda_science_my.position((vial > 0) ? m_clockwise : m_counterclockwise));
                hal::print<1024>(terminal, "User Inputted Delay: %s\n",readings.data);
                hal::print<1024>(terminal, "Revolver Start, Moving %d Vials\n", vial);
                vial = (vial > 0) ? vial : -vial;

                while (count < vial) 
                {
                    hallState = magnet_my.level().value().state;
                    hal::delay(clock,std::chrono::milliseconds(m_delay));
                    hallStateDelayed = magnet_my.level().value().state;
                    
                    if (hallState != hallStateDelayed)
                    {
                        ++count;
                        hal::print<1024>(terminal, "Vials Passed: %d\n", count);
                    }
                }
                
                HAL_CHECK(rotunda_science_my.position(m_stop));
                hal::print<1024>(terminal, "Revolver Stop\n");
                
                count = -1;
                result = std::string_view();
            } 
        }
    }
}