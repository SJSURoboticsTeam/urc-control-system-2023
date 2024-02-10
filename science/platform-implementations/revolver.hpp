#pragma once
#include <libhal/servo.hpp>
#include <libhal/input_pin.hpp>

namespace sjsu::science {

    class revolver {
        private:
            int position;
            hal::servo& servo;
            hal::input_pin& hall_effect_input_pin;
            
            

        public:
            enum class positions{
                SAMPLE_DEPOSIT,
                MOLISCH_REAGENT,
                SULFURIC_ACID,
                BIURET_REAGENT,
                OBSERVATION
            };
            static hal::result<revolver> create(hal::servo& p_servo, hal::input_pin& p_hall_effect_input_pin);
            hal::status rotate_vial_to_position(positions position, int vial);
    };

}