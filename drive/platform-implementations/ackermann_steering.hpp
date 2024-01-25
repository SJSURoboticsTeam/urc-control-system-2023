#pragma once 

#include "./vector_2.hpp"
#include <span>

namespace sjsu::drive {

struct wheel_setting {
    float angle; // 0 is straight forward.
    float wheel_speed;
};

namespace ackermann_steering {
    /**
     * @brief Calculate the angles of each wheel in ackermann steering.
     * 
     * @param turning_circle_center Location of the turning circle center
     * @param wheel_locations Location of each of the wheels.
     * @param wheel_setting_results This span will be filled with the calculated angles
     */
    void calculate_wheel_angles(vector2 turning_circle_center, std::span<vector2> wheel_locations, std::span<wheel_setting> wheel_setting_results);

    /**
     * @brief Calculate the speeds of each wheel in ackermann steering.
     * 
     * @param wheel_speed Speed of a imaginary wheel. Note this is not a speed of any of the actual wheel. It is the speed of an imaginary wheel, a distance of 1 unit away from the origin, which is also perpendicular to the steering axis.
     * @param turning_circle_center Location of the turning circle center
     * @param wheel_locations Location of each of the wheels.
     * @param wheel_setting_results this span will be filled with the calculated speeds.
     */
    void calculate_wheel_speed(float wheel_speed, vector2 turning_circle_center, std::span<vector2> wheel_locations, std::span<wheel_setting> wheel_setting_results);


    /**
     * @brief Calculate the speeds of each wheel in ackermann steering with some constraints.
     * If the `wheel_speed` and `turning_circle_center` result in an angular speed greater than `maximum_angular_speed`, the speed will 
     * be calculated using the `maximum_angular_speed` and `turning_circle_center`. If the `wheel_speed` is greater than `maximum_wheel_speed`,
     * `wheel_speed` is clamped down to `maximum_wheel_speed`.
     * 
     * @param wheel_speed Speed of a imaginary wheel. Note this is not a speed of any of the actual wheel. It is the speed of an imaginary wheel, a distance of 1 unit away from the origin, which is also perpendicular to the steering axis.
     * @param maximum_angular_speed Maximum angular speed.
     * @param maximum_wheel_speed Maximum speed of the imaginary wheel.
     * @param turning_circle_center Location of the turning circle center
     * @param wheel_locations Location of each of the wheels.
     * @param wheel_setting_results this span will be filled with the calculated speeds.
     */
    void calculate_wheel_speed_with_constraints(float wheel_speed, float maximum_angular_speed, float maximum_wheel_speed, vector2 turning_circle_center, std::span<vector2> wheel_locations, std::span<wheel_setting> wheel_setting_results);

};

}