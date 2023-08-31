#pragma once 

#include <libhal/motor.hpp>
#include <libhal-util/units.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::drive {

class print_motor : public hal::motor {
public:
    static hal::result<print_motor> create(hal::serial& p_console) {
        return print_motor(p_console);
    }
    
private:
    print_motor(hal::serial& p_console) : m_console(&p_console)
    {
    }

    hal::result<hal::motor::power_t> driver_power(hal::rpm p_speed) override {
        hal::print<10>(*m_console, "speed: %d\n", p_speed);
        return hal::motor::power_t{};
    }

    hal::serial* m_console;
};

}
