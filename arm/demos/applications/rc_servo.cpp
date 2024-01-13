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
#include <libhal-pca/pca9685.hpp>
using namespace hal::literals;
using namespace std::chrono_literals;

namespace sjsu::arm {
hal::status application(sjsu::arm::application_framework& p_framework) {
    auto& end_effector_my = *p_framework.end_effector;
    auto& clock = *p_framework.clock;
    auto& terminal = *p_framework.terminal;
    while (true) {
        hal::delay(clock, 3000ms);
        hal::print(terminal, "going to 180 deg\n");
        HAL_CHECK(end_effector_my.position(110.0_deg));
        hal::print(terminal, "going to 0 deg\n");
        hal::delay(clock, 3000ms);

        HAL_CHECK(end_effector_my.position(0.0_deg));
        // HAL_CHECK(end_effector_my.duty_cycle(0.5));
    }


    return hal::success();
}


}