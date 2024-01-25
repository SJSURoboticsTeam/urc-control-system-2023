
#include "geared_servo.hpp"
#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::arm {

class geared_servo : public hal::servo {
public:
    static hal::result<geared_servo> create(hal::servo& p_servo, int p_gear_ratio) {
        return geared_servo(p_servo, p_gear_ratio);
    }

    hal::degrees get_gear_ratio() {
        return m_gear_ratio;
    }

    void set_gear_ratio(int p_gear_ratio) {
        m_gear_ratio = p_gear_ratio;
    }
    
private:
    geared_servo(hal::servo& p_servo, int p_gear_ratio) : m_servo(&p_servo), m_gear_ratio(p_gear_ratio) 
    {
    }

    hal::result<hal::servo::position_t> driver_position(hal::degrees p_position) override {
        return HAL_CHECK(m_servo->position(p_position * m_gear_ratio));
    }

    int m_gear_ratio;
    hal::servo* m_servo = nullptr;
};

}
