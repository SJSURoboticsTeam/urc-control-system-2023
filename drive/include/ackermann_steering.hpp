#pragma once 

#include "./vector2.hpp"
#include <span>
#include <libhal/units.hpp>

namespace sjsu::drive {

struct wheel_setting {
    hal::degrees angle; // 0 is straight forward.
    hal::rpm wheel_speed;
};

/**
 * @brief A utility that calculates wheel rotations and speeds for ackermann steering
 * 
 * @warning Maximum angular velocity can only be used if the scale factors for these members are set correctly. (They are currently aren't)
 * 
 */
class ackermann_steering {
    public:
        ackermann_steering(std::span<vector2> p_wheel_locations, std::span<wheel_setting> p_wheel_settings_span, hal::rpm p_maximum_wheel_speed, hal::rpm p_maximum_angular_velocity);

        std::span<wheel_setting> get_wheel_settings();

        /**
         * @brief Calculate the angles and wheel speeds of all the wheels configured in this object.
         * 
         * @param p_signed_turning_radius Location of the turning circle center.
         * @param p_heading
         * @param p_wheel_speed Speed of an imaginary wheel
         * @return std::span<wheel_setting> 
         */
        std::span<wheel_setting> calculate_wheel_settings(float p_signed_turning_radius, hal::degrees p_heading, hal::rpm p_wheel_speed);
    private:
        std::span<vector2> m_wheel_locations;
        std::span<wheel_setting> m_wheel_settings;
        hal::rpm m_maximum_wheel_speed, m_maximum_angular_velocity;
};

}