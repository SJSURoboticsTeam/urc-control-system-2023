#include <libhal-armcortex/system_control.hpp>


#include "hardware_map.hpp"

hal::result<science::hardware_map> initialize_target() {

    return science::hardware_map {
        .reset = []() { hal::cortex_m::system_control::reset(); },
    };
}