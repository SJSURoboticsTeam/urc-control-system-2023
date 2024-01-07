#include <rc_servo.hpp> 
#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

namespace sjsu::arm {
hal::status application(sjsu::arm::application_framework& p_framework) {
    auto& end_effector_my = *p_framework.end_effector;
    auto& clock = *p_framework.clock;

    while (true) {
        HAL_CHECK(end_effector_my.position(30deg));
        hal::delay(clock, 500ms);
    }

    return hal::success();
}


}