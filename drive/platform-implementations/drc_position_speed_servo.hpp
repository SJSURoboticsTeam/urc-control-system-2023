#pragma once 
#include "./position_speed_servo.hpp"
#include <libhal/servo.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::drive {
class drc_position_speed_servo : public position_speed_servo {
    public:
        inline hal::result<drc_position_speed_servo> make_position_speed_servo(hal::rmd::drc& p_drc, hal::rpm p_max_speed) {
            drc_position_speed_servo servo(p_drc, p_max_speed);
            return servo;
        }
    private:
        drc_position_speed_servo(hal::rmd::drc& p_drc, hal::rpm p_max_speed);

        hal::status driver_position_speed(hal::degrees p_position, hal::rpm p_speed);

        hal::rmd::drc* m_motor;
        hal::rpm m_max_speed;
    
};
}