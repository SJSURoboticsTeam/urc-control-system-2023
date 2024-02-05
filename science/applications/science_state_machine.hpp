#pragma once

namespace sjsu::science::states{
    enum science_states{
        GET_SAMPLES,
        MOLISCH_TEST,
        BIURWT_TEST,
        OBSERVE_TEST
    };

    hal::status pump_dio_water();

    hal::status mix_solution();

    hal::status pump_to_vial();
    
    hal::status pump_reagents();

    hal::status rotate_vials();

    hal::status turn_on_light();
    
    hal::status read_color_sensor();




    


 }