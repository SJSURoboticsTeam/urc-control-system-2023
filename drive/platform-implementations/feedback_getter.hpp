#pragma once
#include "./mission_control.hpp"
#include <libhal-rmd/drc.hpp>
#include "../platform-implementations/offset_servo.hpp"
#include "../platform-implementations/calibration_settings.hpp"


namespace sjsu::drive {
struct feedback_getter {

    hal::rmd::drc * fl_steer;
    hal::rmd::drc * fr_steer;
    hal::rmd::drc * b_steer;
    
    hal::rmd::drc * fl_prop;
    hal::rmd::drc * fr_prop;
    hal::rmd::drc * b_prop;

    float fl_offset;
    float fr_offset;
    float b_offset;

    inline mission_control::mc_feedback::motor_feedback get_motor_feedback(hal::rmd::drc * motor, float off) {
        mission_control::mc_feedback::motor_feedback feedback;
        feedback.angle = static_cast<float>(motor->feedback().encoder) / angle_correction_factor  - off;
        feedback.speed = motor->feedback().speed();
        feedback.current = static_cast<float>(motor->feedback().raw_current) * 0.01;
        // feedback.temperature = static_cast<float>(motor->feedback().raw_motor_temperature);
        return feedback;
    }

    inline mission_control::mc_feedback get_feedback() {
        mission_control::mc_feedback feedback;

        feedback.fl.propulsion = get_motor_feedback(fl_prop, 0);
        feedback.fr.propulsion = get_motor_feedback(fr_prop, 0);
        feedback.b.propulsion = get_motor_feedback(b_prop, 0);

        feedback.fl.steering = get_motor_feedback(fl_steer, fl_offset);
        feedback.fr.steering = get_motor_feedback(fr_steer, fr_offset);
        feedback.b.steering = get_motor_feedback(b_steer, b_offset);

        return feedback;
    }


};
};