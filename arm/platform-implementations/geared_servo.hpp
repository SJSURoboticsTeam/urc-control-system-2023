#pragma once 

#include <libhal/servo.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::arm {

/**
 * @brief This is effectively a wrapper around a servo
 *        whose outputs shaft has been geared up or down.
 * 
 */
class geared_servo : public hal::servo {
public:
    /**
     * @brief Create a geared servo
     * 
     * @param p_servo 
     * @param p_gear_ratio 
     * @return hal::result<geared_servo> 
     */
    static hal::result<geared_servo> create(hal::servo& p_servo, int p_gear_ratio);

    hal::degrees get_gear_ratio();

    void set_gear_ratio(int p_gear_ratio);
    
private:
    geared_servo(hal::servo& p_servo, int p_gear_ratio);
    
    hal::result<hal::servo::position_t> driver_position(hal::degrees p_position);

    int m_gear_ratio;
    hal::servo* m_servo = nullptr;
};

}
