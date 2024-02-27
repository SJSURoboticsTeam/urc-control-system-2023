#pragma once
#include "./mission_control.hpp"
#include <libhal-rmd/mc_x.hpp>

namespace sjsu::arm {
struct feedback_getter {

    hal::rmd::mc_x * rotunda;
    hal::rmd::mc_x * shoulder;
    hal::rmd::mc_x * elbow;
    hal::rmd::mc_x * wrist_left;
    hal::rmd::mc_x * wrist_right;

    inline mission_control::mc_feedback::motor_feedback get_motor_feedback(hal::rmd::mc_x * motor) {
        mission_control::mc_feedback::motor_feedback feedback;
        feedback.angle = static_cast<float>(motor->feedback().encoder);
        feedback.speed = motor->feedback().speed();
        feedback.current = static_cast<float>(motor->feedback().raw_current) * 0.01;
        feedback.temperature = static_cast<float>(motor->feedback().raw_motor_temperature);
        return feedback;
    }

    inline mission_control::mc_feedback get_feedback() {
        mission_control::mc_feedback feedback;

        feedback.rotunda = get_motor_feedback(rotunda);
        feedback.shoulder = get_motor_feedback(shoulder);
        feedback.elbow = get_motor_feedback(elbow);
        feedback.wristLeft = get_motor_feedback(wrist_left);
        feedback.wristRight = get_motor_feedback(wrist_right);

        return feedback;
    }


};
};
