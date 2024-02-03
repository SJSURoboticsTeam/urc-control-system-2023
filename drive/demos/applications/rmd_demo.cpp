// Currently doesnt work. Potential UART issue

#include <libhal-rmd/drc.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/input_pin.hpp>

#include "../../include/pwm_relay.hpp"
#include "../application.hpp"

namespace sjsu::drive {

hal::status application(application_framework& p_framework) 
{
    using namespace std::chrono_literals;
    using namespace hal::literals;

    auto& can = *p_framework.can;
    auto& terminal = *p_framework.terminal;
    auto& clock = *p_framework.steady_clock;
    auto& relay = *p_framework.motor_relay;

    HAL_CHECK(hal::write(terminal, "Starting demo...\n"));

    HAL_CHECK(relay.toggle(true));

    static auto router = HAL_CHECK(hal::can_router::create(can));

    hal::delay(clock, 1s);

    static auto motor = HAL_CHECK(hal::rmd::drc::create(router, clock, 6.0, 0x141));

    hal::delay(clock, 1s);
    
    HAL_CHECK(motor.velocity_control(5.0_rpm));

    while (true) {
        hal::delay(clock, 1s);
    }

    return hal::success();
}

}
