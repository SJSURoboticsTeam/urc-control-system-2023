

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>
#include "./position_speed_servo.hpp"
#include "./position_speed_servo_offset.hpp"

namespace sjsu::drive {

    hal::result<position_speed_offset_servo> position_speed_offset_servo::create(position_speed_servo& p_servo, hal::degrees p_offset) {
        return position_speed_offset_servo(p_servo, p_offset);
    }

    hal::degrees position_speed_offset_servo::get_offset() {
        return m_offset;
    }

    void position_speed_offset_servo::set_offset(hal::degrees p_offset) {
        m_offset = p_offset;
    }
    
    position_speed_offset_servo::position_speed_offset_servo(position_speed_servo& p_servo, hal::degrees p_offset) : m_servo(&p_servo), m_offset(p_offset) 
    {
    }

    hal::status position_speed_offset_servo::driver_position_speed(hal::degrees p_position, hal::rpm p_speed) {
        m_servo->position_speed(p_position + m_offset, p_speed);
        return hal::success();
    }

}
