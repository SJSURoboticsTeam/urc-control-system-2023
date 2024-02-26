

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>
#include "./position_speed_servo.hpp"

namespace sjsu::drive {

class position_speed_offset_servo : public position_speed_servo {
public:
    static hal::result<position_speed_offset_servo> create(position_speed_servo& p_servo, hal::degrees p_offset) {
        return position_speed_offset_servo(p_servo, p_offset);
    }

    hal::degrees get_offset() {
        return m_offset;
    }

    void set_offset(hal::degrees p_offset) {
        m_offset = p_offset;
    }
    
private:
    position_speed_offset_servo(position_speed_servo& p_servo, hal::degrees p_offset) : m_servo(&p_servo), m_offset(p_offset) 
    {
    }

    hal::status driver_position_speed(hal::degrees p_position, hal::rpm p_speed) {
        m_servo->position_speed(p_position + m_offset, p_speed);
        return hal::success();
    }

    hal::degrees m_offset;
    position_speed_servo* m_servo = nullptr;
};

}
