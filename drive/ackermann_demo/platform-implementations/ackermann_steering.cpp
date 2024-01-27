#include "./ackermann_steering.hpp"
#include <cmath>

using namespace sjsu::drive;

ackermann_steering::ackermann_steering(std::span<vector2> p_wheel_locations, hal::rpm p_maximum_wheel_speed, hal::rpm p_maximum_angular_velocity) {
    m_wheel_locations = p_wheel_locations;
    m_maximum_wheel_speed = p_maximum_wheel_speed;
    m_maximum_angular_velocity = p_maximum_angular_velocity;
}

// hal::degrees ackermann_steering::calculate_wheel_angle(vector2 p_turning_circle_center, vector2 p_wheel_location) {
//     vector2 direction = wheel_location - turning_circle_center;
//     vector2 wheel_direction = vector2::rotate_90_ccw(direction);
//     return vector2::bearing(wheel_direction);
// }

std::span<wheel_setting> ackermann_steering::get_wheel_settings() {
    return m_wheel_settings;
}

std::span<wheel_setting> ackermann_steering::calculate_wheel_settings(float p_signed_turning_radius, hal::degrees p_heading, hal::rpm p_speed) {
    // hal::rpm max_speed_of_wheels = 0;
    
    float turning_radius = std::abs(p_signed_turning_radius);
    float imaginary_wheel_distance = sqrt(1 + turning_radius * turning_radius);

    // Bounds checking:
    // float maximum_speed_due_to_angular_speed_constraint = m_maximum_angular_velocity * imaginary_wheel_distance;
    // p_speed = std::clamp(p_speed, -maximum_speed_due_to_angular_speed_constraint, maximum_speed_due_to_angular_speed_constraint);
    p_speed = std::clamp(p_speed, -m_maximum_wheel_speed, m_maximum_wheel_speed);



    vector2 turning_circle_center = vector2::rotate_90_cw(vector2::from_bearing(turning_radius, p_heading * std::numbers::pi / 180));

    for(size_t i = 0; i < m_wheel_locations.size(); i ++) {
        
        if(std::isinf(p_signed_turning_radius)) {
            // Translate mode;
            m_wheel_settings[i].angle = p_heading * 180 / std::numbers::pi;
            m_wheel_settings[i].wheel_speed = p_speed;
        }else {
            vector2 direction = m_wheel_locations[i] - turning_circle_center;
            if(p_signed_turning_radius < 0) {
                m_wheel_settings[i].angle = vector2::bearing_angle(vector2::rotate_90_ccw(direction)) * 180 / std::numbers::pi;
            }else {
                m_wheel_settings[i].angle = vector2::bearing_angle(vector2::rotate_90_cw(direction)) * 180 / std::numbers::pi;
            }

            float wheel_distance = vector2::length(wheel_distance);
            m_wheel_settings[i].wheel_speed = p_speed * wheel_distance / imaginary_wheel_distance;
        }

        // max_speed_of_wheels = std::max(max_speed_of_wheels, std::abs(m_wheel_settings[i].wheel_speed));
    }

    // if(m_maximum_wheel_speed < max_speed_of_wheels) {
    //     float wheel_speed_scale_factor = m_maximum_wheel_speed / max_speed_of_wheels;
    //     for(size_t i = 0; i < m_wheel_settings.size(); i ++) {
    //         m_wheel_settings[i].wheel_speed *= wheel_speed_scale_factor;
    //     }
    // }

    return m_wheel_settings;
}
