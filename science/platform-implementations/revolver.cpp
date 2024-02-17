#pragma once
#include "revolver.hpp"

namespace sjsu::science {
    
    revolver::revolver(hal::motor& p_motor, hal::input_pin& p_hall_effect_input_pin): 
        motor(p_motor),
        hall_effect_input_pin(p_hall_effect_input_pin) {}

    hal::result<revolver> revolver::create(hal::motor& p_motor, hal::input_pin& p_hall_effect_input_pin) {
        return revolver(p_motor,p_hall_effect_input_pin);
    }
    hal::status revolver::rotate_vial_to_position(revolver::positions position, int vial) {

        

    }
    hal::status revolver::move_one(bool forward) {
        motor.power(k_motor_rotate_power);
        while (hall_effect_input_pin.level()) {}
    }
        

}