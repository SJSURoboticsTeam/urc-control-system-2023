#pragma once
#include <libhal/motor.hpp>
#include <libhal/input_pin.hpp>

namespace sjsu::science {

    class revolver {
        private:
            static const float k_motor_rotate_power = 0.01;
            int position = 0; //vial in sample position
            hal::motor& motor;
            hal::input_pin& hall_effect_input_pin;
            revolver(hal::motor& p_motor, hal::input_pin& p_hall_effect_input_pin);
            hal::status move_one(bool forward);

        public:
            enum class positions{
                SAMPLE_DEPOSIT=0,
                MOLISCH_REAGENT,
                SULFURIC_ACID,
                BIURET_REAGENT,
                OBSERVATION
            };
            static hal::result<revolver> create(hal::motor& p_motor, hal::input_pin& p_hall_effect_input_pin);
            hal::status rotate_vial_to_position(positions position, int vial);
    };

}