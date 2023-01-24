#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "libhal_configuration/hardware_map.hpp"
#include "implementations/mq4_methane_sensor.hpp"

// Defined to use custom libhal units such as volts or milliseconds
using namespace hal::literals;
using namespace std::chrono_literals;

hal::status science_app(science::hardware_map& p_map) {
    // science robot entry point here. This function may yield an error.
    while (true) {
        // demo for methane sensor driver.
        auto methane_driver = Mq4MethaneSensor(p_map.is_methane, p_map.methane_level);
        float methane_value = HAL_CHECK(methane_driver.read_methane_level());
        bool is_methane = HAL_CHECK(methane_driver.detect_methane());

        if (is_methane) {
            hal::print<64>(*p_map.science_serial, "CH4: %f\n", methane_value);
        } else {
            hal::print<64>(*p_map.science_serial, "No methane detected.\n");
        }
        HAL_CHECK(hal::delay(*p_map.clock, 100ms));

    }
    return hal::success();
}

int main() {
    // Initializing the data section initializes global and static variables and
    // is required for the standard C library to run.
    hal::cortex_m::initialize_data_section();
    hal::cortex_m::system_control::initialize_floating_point_unit();

    auto init_result = initialize_target();
    if (!init_result)
        hal::halt();
    
    auto p_map = init_result.value();

    

    auto app_result = science_app(p_map);
    
    if (!app_result) {
    p_map.reset();
    } else {
        hal::halt(); // TODO: implement error handling (right now just enters an infinite loop)
    }
  

    return 0;
}

namespace boost {
void throw_exception([[maybe_unused]] std::exception const& p_error)
{
  std::abort();
}
} // namespace boost
