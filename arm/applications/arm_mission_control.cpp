#pragma once

#include "arm_mission_control.hpp"

namespace sjsu::arm {

class arm_mission_control : public mission_control {
    hal::result<mc_commands> get_commands() {
        // read some stuff here and place it into commands
        mc_commands commands;
        return commands
    }
    hal::status send_commands() {
        // write information here
        
        return hall::success()
    }
    private:

}

} 