#pragma once 

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::drive {

class offset_servo : public hal::servo {
public:
    static hal::result<offset_servo> create(hal::servo& p_servo, hal::degrees p_offset);

    hal::degrees get_offset();

    void set_offset(hal::degrees p_offset);
    
private:
    offset_servo(hal::servo& p_servo, hal::degrees p_offset);

    hal::result<hal::servo::position_t> driver_position(hal::degrees p_position) override;

    hal::degrees m_offset;
    hal::servo* m_servo = nullptr;
};

}
