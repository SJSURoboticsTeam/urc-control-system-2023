#pragma once 

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>
#include <libhal-util/serial.hpp>

namespace sjsu::drive {

class print_servo : public hal::servo {
public:
    static print_servo create(hal::serial& p_console, hal::servo& p_servo) {
        return print_servo(p_console, p_servo);
    }
    
private:
    print_servo(hal::serial& p_console, hal::servo& p_servo) : m_console(&p_console) , m_servo(&p_servo)
    {
    }

    hal::servo::position_t driver_position(hal::degrees p_position) override {
        hal::print<20>(*m_console, "position: %f\n", static_cast<float>(p_position));
        return m_servo->position(p_position);
    }
    
    hal::serial* m_console;
    hal::servo* m_servo;
};

}
