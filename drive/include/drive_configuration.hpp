#pragma once


namespace sjsu::drive {

    struct drive_configuration {
        float steering_angle = 0.0;
        float wheel_heading = 0.0;
        float wheel_speed = 0.0;

        constexpr drive_configuration() {}
        constexpr drive_configuration(float p_steering_angle, float p_wheel_heading, float p_wheel_speed) : 
            steering_angle(p_steering_angle),
            wheel_heading(p_wheel_heading),
            wheel_speed(p_wheel_speed) {}
    };
}