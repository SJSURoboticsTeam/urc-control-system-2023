#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-soft/rc_servo.hpp>
#include "../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

namespace sjsu::arm {
hal::status application(sjsu::arm::application_framework& p_framework) {

    return hal::success();
}


}