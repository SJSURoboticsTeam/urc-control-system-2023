#pragma once

#include "../include/mission_control.hpp"

#include <libhal-util/serial.hpp>
#include <libhal/motor.hpp>
#include <libhal/serial.hpp>
#include <libhal/servo.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::drive{

class print_mission_control : public mission_control 
{
public:

    static hal::result<print_mission_control> create(hal::serial& p_console)
    {
        return print_mission_control(p_console);
    }

    hal::result<mc_commands> get_command(hal::function_ref<hal::timeout_function> p_timeout)
    {
        return impl_get_command(p_timeout);
    }

private:

    print_mission_control(hal::serial& p_console) : m_console(&p_console) 
    {
    }

    hal::result<mc_commands> impl_get_command(hal::function_ref<hal::timeout_function> p_timeout) {
       hal::print(*m_console, "getting command");
       return mc_commands{};
    }
    hal::serial* m_console;
};

}
