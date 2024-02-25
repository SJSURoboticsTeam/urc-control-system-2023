#pragma once
#include <application.hpp>

namespace sjsu::science{
    class science_state_machine{

        enum class science_states{
            GET_SAMPLES,
            MOLISCH_TEST,
            BIURET_TEST,
            RESET
        };
        // auto sm = science_state_machine()
        // science_state_machine::create()

        sjsu::science::application_framework& hardware;
        vial2_position current_position= SAMPLE; 
        science_states current_state= GET_SAMPLES;
        static hal::result<science_state_machine> create(sjsu::science::application_framework& p_application)
        hal::status run_state_machine(science_states current_state);
        hal::status pump_dio_water();
        hal::status mix_solution();
        hal::status pump_sample(); 
        hal::status pump_to_vial();
        hal::status pump_reagents();
        hal::status pump_molish_reagents(); 
        hal::status pump_sulfiruc_acid();
        hal::status pump_biuret_reagents(); 
        hal::status containment_reset();
        hal::status containment_reset(); 
        hal::status turn_off_pumps();

    

        

    };

 }