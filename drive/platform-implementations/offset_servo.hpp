#pragma once 

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::drive {

class offset_servo : public servo {
public:
    hal::result<offset_servo> create(hal::servo p_servo, hal::degrees p_offset) {
        return offset_servo(p_servo, p_offset);
    }

private:
    offset_servo(hal::servo p_servo, hal::degrees p_offset) : m_servo(p_servo), m_offset(p_offset) 
    {
    }

    result<position_t> driver_position(hal::degrees p_position) override {
        return HAL_CHECK(m_servo.position(p_position + m_offset));
    }

    hal::degrees m_offset;
    hal::servo m_servo;
};

}
