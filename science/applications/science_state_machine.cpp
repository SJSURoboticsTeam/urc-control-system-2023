#include "science_state_machine.hpp"
#include "revolver.hpp"
#include <libhal/servo.hpp> 
namespace sjsu::science{
    hal::status run_state_machine(){
        switch(state){
            case science_states::GET_SAMPLES:
                pump_dio_water();

                mix_solution();
                pump_to_vial();

                rotate_vial_to_position()
                pump_to_vial(); 
                break;
            case science_states::MOLISCH_TEST:
                pump_reagents();
                rotate_vial_to_position()
                turn_on_light();
                rotate_vial_position(); 
                read_color_sensor(); 
                break;
            case science_states::BIURWT_TEST:
                pump_reagents();
                rotate_vial_to_position(); 
                turn_on_light();
                rotate_vial_position();
                read_color_sensor(); 
                break;
            case science_states::RESET:
                rotate_vial_to_position(); 
                break; 
        }
        //check if you continue and containment reset 
    }
}
//check substates
//exit if molish test fails 
//


