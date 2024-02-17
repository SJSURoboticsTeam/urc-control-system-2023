#include "science_state_machine.hpp"
#include "revolver.cpp"
#include <libhal/servo.hpp> 
namespace sjsu::science{
    hal::result<science_states> science_state_machine::run_state_machine(science_states state){
        switch(current_state){
            case science_state_machine::science_states::GET_SAMPLES:
                pump_dio_water();
                mix_solution();
                pump_to_vial(1);
                revolverMoveVials(1); // move to vial 2
                pump_to_vial(2); 
                current_state= science_states::MOLISCH_TEST;
                break; 
            case science_state_machine::science_states::MOLISCH_TEST:
                pump_reagents(1);
                current_position = vial2_position::MOLISCH; 
                revolverMoveVials(2); //move vial 1 to camera/color sensor 
                current_position= vial2_position::BIURET; 
                up_and_down(); //moves color sensor up and down so it can read the color
                read_color_sensor(); //could be removed
                if(read_color_sensor_purple()){
                    current_state= science_states::BIURET_TEST;
                }
                else{
                    current_state= science_states::GET_SAMPLES;
                }
                break;
            case science_state_machine::science_states::BIURET_TEST:
                pump_reagents(2); // pump into vial 2
                revolverMoveVials(1); // rotate to camera/color_sensor 
                current_position= vial2_position::CAMERA;
                up_and_down(); 
                read_color_sensor(); 
                current_state= science_states::RESET;
                break;
            case science_state_machine::science_states::RESET:
                if((vial2_position-2)<0){
                    revolverMoveVials(1);
                }
                else{
                    revolverMoveVials(-(vial2_position-2));
                }
                current_state= science_states::GET_SAMPLES;
                break; 
        }
        return state; 
        
    }
    hal::status science_state_machine::containment_reset(){
        turn_off_pumps(); 
        state=science_states::GET_SAMPLES; 
    }
}


