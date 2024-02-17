#pragma once

namespace sjsu::science{
    class science_state_machine{

        enum class science_states{
            GET_SAMPLES,
            MOLISCH_TEST,
            BIURET_TEST,
            RESET
        };

        hal::result<bool> check_sample(); 
        hal::status run_state_machine(science_states state);
        hal::status pump_dio_water();
        hal::status mix_solution();
        hal::status pump_to_vial();
        hal::status pump_reagents();
        hal::status rotate_vials();
        hal::status turn_on_light();
        hal::status read_color_sensor();//move contnets insdie
        hal::result<bool> get_mission_control_cmnd(); //temporary
        hal::status containment_reset();


        

        

    };

 }