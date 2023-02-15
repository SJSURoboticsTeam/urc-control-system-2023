#pragma once

#include "../implementations/pump_controller.hpp"

namespace science {
    bool suck(science::PumpPwmController& air_pump) {
        return false;
    }

    bool stopSucking(science::PumpPwmController& air_pump) {
        return false;
    }
}