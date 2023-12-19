#pragma once

#include "../include/speed_sensor.hpp"
#include <libhal-rmd/drc.hpp>
#include <libhal-util/units.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::drive {

class print_speed_sensor : public speed_sensor {
public:

    static hal::result<print_speed_sensor> make_speed_sensor(hal::serial& p_console) {
        return print_speed_sensor(p_console);
    }

private:

    print_speed_sensor(hal::serial& p_console) 
    : m_console(&p_console)
    {
    }

    hal::result<speed_sensor::read_t> driver_read() {
        using namespace hal::literals;
        
        auto speed = 0.0_rpm;
        hal::print<100>(*m_console, "speed read: %f\n", speed); 

        return speed_sensor::read_t{.speed = 0.0_rpm};
    }    
    hal::serial* m_console;
};
}
