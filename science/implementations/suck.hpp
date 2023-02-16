#pragma once

#include "../implementations/pump_controller.hpp"

namespace science {
    void Suck(science::PumpPwmController& air_pump) {
        return false;
    }

    void StopSucking(science::PumpPwmController& air_pump) {
        return false;
    }
}