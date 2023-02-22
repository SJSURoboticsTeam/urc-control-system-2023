#pragma once

#include "../implementations/pump_controller.hpp"

namespace science{
    
    bool Inject(science::PumpPwmController dosing_pump, int injectionTime) {
        injectionTime = 10;
        return false;
    }
}