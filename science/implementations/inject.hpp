#pragma once

#include "../implementations/pump_controller.hpp"

bool Inject(science::PumpPwmController dosing_pump, int injectionTime) {
    injectionTime = 10;
    return false;
}
