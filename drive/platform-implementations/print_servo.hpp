#pragma once 

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::drive {

class print_servo : public hal::servo {
public:
    static hal::result<print_servo> create(hal::serial& p_console) {
        return print_servo(p_console);
    }
    
private:
    print_servo(hal::serial& p_console) : m_console(&p_console)
    {
    }

    hal::result<hal::servo::position_t> driver_position(hal::degrees p_position) override {
        hal::print<20>(*m_console, "position: %f\n", static_cast<float>(p_position));
        return hal::servo::position_t{};
    }
    
    hal::serial* m_console;
};

}
