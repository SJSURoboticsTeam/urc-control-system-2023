#include <libhal-rmd/drc.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>

#include "../../platform-implementations/relay.hpp"
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

    auto router = HAL_CHECK(hal::can_router::create(can));

    auto motor = HAL_CHECK(hal::rmd::drc::create(router, clock, 15.0, 0x141));

    HAL_CHECK(hal::delay(clock, 1s));

    HAL_CHECK(hal::write(terminal, "Starting!\n"));

    HAL_CHECK(motor.velocity_control(5.0_rpm));

    while (true) {
        HAL_CHECK(hal::delay(clock, 1s));
    }

    return hal::success();
}

}
