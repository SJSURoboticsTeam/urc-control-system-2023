#pragma once


namespace sjsu::drive {
    /**
     * @brief This structure defines a valid steering configuration for the drive system.
     * 
     */
    struct drive_configuration {
        float steering_angle = 0.0;
        float wheel_heading = 0.0;
        float wheel_speed = 0.0;
    };
}