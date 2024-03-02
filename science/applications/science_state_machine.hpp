#pragma once
#include "application.hpp"

namespace sjsu::science{
    class science_state_machine{
        private:
        science_state_machine(application_framework& p_application);
        application_framework& hardware;

        public:
        enum class science_states{
            GET_SAMPLES,
            MOLISCH_TEST,
            BIURET_TEST,
            RESET
        };
        // auto sm = science_state_machine()
        // science_state_machine::create()

    
        // vial2_position current_position= SAMPLE; 
        // science_states current_state= GET_SAMPLES;

        static hal::result<science_state_machine> create(application_framework& p_application);
        hal::status run_state_machine(science_states current_state);

        hal::status mix_solution();
        hal::status turn_on_pump(auto pump);
        hal::status move_sample(int position);
        hal::status containment_reset(); 
        // hal::status turn_off_pumps();   

    };

 }