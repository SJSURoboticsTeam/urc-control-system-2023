#pragma once
#include <libhal/functional.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/adc.hpp>

namespace science {

struct hardware_map {
    hal::callback<void()> reset; // reset function
};

hal::result<science::hardware_map> initialize_target();

} // namespace science