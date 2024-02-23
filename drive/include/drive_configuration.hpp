#pragma once


namespace sjsu::drive {

    struct drive_configuration {
        float steering_angle = 0.0;
        float wheel_heading = 0.0;
        float wheel_speed = 0.0;

        constexpr drive_configuration(float p_steering_angle, float p_wheel_heading, float p_wheel_speed) : 
            steering_angle(p_steering_angle),
            wheel_heading(p_wheel_heading),
            wheel_speed(p_wheel_speed) {}
    };

    class drive_configuration_updater {
        public: 
            void set_target(drive_configuration p_target);
            void update(float dt);
            drive_configuration get_current();
            drive_configuration get_target();
            drive_configuration get_delta();
        private:
            drive_configuration m_current;
            drive_configuration m_target;
            drive_configuration m_kP = drive_configuration(1, 10, 20);
            drive_configuration m_delta;
            drive_configuration m_max_delta = drive_configuration(40, 360, 50);
    };
}