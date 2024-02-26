#include "./drc_position_speed_servo.hpp"

namespace sjsu::drive {

    drc_position_speed_servo::drc_position_speed_servo(hal::rmd::drc& p_drc, hal::rpm p_max_speed) {
        m_motor = &p_drc;
        m_max_speed = p_max_speed;
    }

    hal::status drc_position_speed_servo::driver_position_speed(hal::degrees p_position, hal::rpm p_speed) {
        HAL_CHECK(m_motor->position_control(p_position, p_speed));
        return hal::success();
    }
}