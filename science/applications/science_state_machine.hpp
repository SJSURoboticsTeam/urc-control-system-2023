#pragma once

namespace sjsu::science{
    class science_state_machine{

        enum class science_states{
            GET_SAMPLES,
            MOLISCH_TEST,
            BIURET_TEST,
            RESET
        };
        enum vial2_position{
            SAMPLE=1, 
            MOLISCH, 
            SULFURIC, 
            BIURET,
            CAMERA
        };
        positions current_position= SAMPLE; 
        science_states current_state= GET_SAMPLES; 
        hal::status run_state_machine(science_states current_state);
        hal::status pump_dio_water();
        hal::status mix_solution();
        hal::status pump_to_vial();
        hal::status pump_reagents();
        hal::status containment_reset();
        hal::status up_and_down(); 
        hal::status containment_reset(); 
        hal::status turn_off_pumps();

    

        

    };

 }