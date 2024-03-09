#include "science_state_machine.hpp"

using namespace std::chrono_literals;

namespace sjsu::science{

    science_state_machine::science_state_machine(application_framework& application) : hardware(application){}

    hal::result<science_state_machine> science_state_machine::create(application_framework& p_application){
        science_state_machine science_state_machine(p_application);
        return science_state_machine;
     }

    hal::status science_state_machine::run_state_machine(science_state_machine::science_states state){
        switch(state){
            case science_state_machine::science_states::GET_SAMPLES:
                mix_solution();
                turn_on_pump(pump_manager::pumps::DEIONIZED_WATER, 5000ms);
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
        return hal::success();
        
    }

    hal::status science_state_machine::turn_on_pump( auto pump, hal::time_duration time){
        auto pump_controller = *hardware.pump_controller;
        pump_controller.pump(pump, time);
        return hal::success();
    }

    

    hal::status science_state_machine::mix_solution(){
        // hardware.mixing_servo.velocity_control(10.0 rpm);
        // hal::delay(hardware.steady_clock, 5000ms);
        return hal::success();
    }

    hal::status science_state_machine::move_sample(int position){
        auto revolver_controller = *hardware.revolver_controller; 
        revolver_controller.revolverMoveVials(position);
        return hal::success(); 
    }
  
}


