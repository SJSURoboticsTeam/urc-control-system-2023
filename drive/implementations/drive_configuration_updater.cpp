#include "../include/drive_configuration_updater.hpp"
#include <algorithm>

namespace sjsu::drive {
    void drive_configuration_updater::set_target(drive_configuration p_target) {
        m_target = p_target;
    }
    void drive_configuration_updater::update(float dt) {
        m_delta.steering_angle = (m_target.steering_angle - m_current.steering_angle) * m_kP.steering_angle;
        m_delta.wheel_heading = (m_target.wheel_heading - m_current.wheel_heading) * m_kP.wheel_heading;
        m_delta.wheel_speed = (m_target.wheel_speed - m_current.wheel_speed) * m_kP.wheel_speed;

        m_delta.steering_angle = std::clamp(m_delta.steering_angle, -m_max_delta.steering_angle, m_max_delta.steering_angle);
        m_delta.wheel_heading = std::clamp(m_delta.wheel_heading, -m_max_delta.wheel_heading, m_max_delta.wheel_heading);
        m_delta.wheel_speed = std::clamp(m_delta.wheel_speed, -m_max_delta.wheel_speed, m_max_delta.wheel_speed);

        m_current.steering_angle += m_delta.steering_angle * dt;
        m_current.wheel_heading += m_delta.wheel_heading * dt;
        m_current.wheel_speed += m_delta.wheel_speed * dt;
    }
    drive_configuration drive_configuration_updater::get_current() {
        return m_current;
    }
    drive_configuration drive_configuration_updater::get_target() {
        return m_target;
    }
    drive_configuration drive_configuration_updater::get_delta() {
        return m_delta;
    }
    void drive_configuration_updater::set_kP(drive_configuration p_kP) {
        m_kP = p_kP;
    }
    void drive_configuration_updater::set_max_delta(drive_configuration p_max_delta) {
        m_max_delta = p_max_delta;
    }
}