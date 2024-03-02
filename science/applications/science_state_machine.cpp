#include "science_state_machine.hpp"
#include <libhal/servo.hpp> 
#
namespace sjsu::science{

     science_state_machine(application_framework& application) : hardware(application){}

     static hal::result<science_state_machine> science_state_machine::create(application_framework& p_application){
        science_state_machine science_state_machine(p_application);
        return science_state_machine;
     }

    hal::status science_state_machine::run_state_machine(science_state_machine::science_states state){
        switch(state){
            case science_state_machine::science_states::GET_SAMPLES:
                mix_solution();
                turn_on_pump(hardware.DEIONIZED_WATER);
                // pump_sample(); 
                // move_sample();
                // pump_sample();
                // move_sample();
                break; 
            case science_state_machine::science_states::MOLISCH_TEST:
                // pump_sample(); 
                // move_sample();
                // pump_sample();
                // move_sample();
                break;
            case science_state_machine::science_states::BIURET_TEST:
                // pump_sample();
                break;
            case science_state_machine::science_states::RESET:
                // if((vial2_position-2)<0){
                //     revolverMoveVials(1);
                // }
                // else{
                //     revolverMoveVials(-(vial2_position-2));
                // }
                // current_state= science_states::GET_SAMPLES;
                break; 
        }
        
    }

    hal::status science_state_machine::turn_on_pump( auto pump)(){
        
    }

    hal::status science_state_machine::mix_solution(){
        HAL_CHECK(hardware.mixing_servo.velocity_control(10.0_rpm));
        hal::delay(hardware.clock, 5000ms);
        return hal::success();
    }

    hal::status science_state_machine::containment_reset(){
        turn_off_pumps(); 
        // state=science_states::GET_SAMPLES; 
    }
}


