#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>

#include "libhal_configuration/hardware_map.hpp"
#include "implementations/mq4_methane_sensor.hpp"

// demos
#include "demos/methane_sensor.hpp"
#include "demos/pump_demo.hpp"

// Defined to use custom libhal units such as volts or milliseconds
using namespace hal::literals;
using namespace std::chrono_literals;

hal::status science_app(science::hardware_map& p_map) {
    // This is for the main science robot. The control flow has not been figured out! Right now
    // We just have some demos for various drivers and components. To run them, include the right header
    // from the demos folder.
    
    HAL_CHECK(demos::pump_demo(p_map));
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
        hal::write(*p_map.science_serial, "A HAL_CHECK has failed.");
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
