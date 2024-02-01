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
    [[maybe_unused]] auto& relay = *p_framework.motor_relay;
    
    HAL_CHECK(hal::write(terminal, "Flag 1 "));

    // HAL_CHECK(relay.toggle(true));
    // if(!relay.toggle(true)) {
    //     HAL_CHECK(hal::write(terminal, "Toggle failed!!\n"));    
    // }
    HAL_CHECK(hal::write(terminal, "Flag 2 "));

    auto router_result = hal::can_router::create(can);

    if (!router_result) {
        HAL_CHECK(hal::write(terminal, "can router failed!\n"));
        while(true) {continue;}
    }

    HAL_CHECK(hal::write(terminal, "Flag 3 "));
    [[maybe_unused]] auto& router = router_result.value();

    hal::delay(clock, 1s);

    // auto motor_result = hal::rmd::drc::create(router, clock, 15.0, 0x141);

    // if (!motor_result) {
    //     HAL_CHECK(hal::write(terminal, "drc create failed!\n"));
    //     while(true) {continue;}
    // }

    // auto& motor = motor_result.value();

    HAL_CHECK(hal::write(terminal, "Flag 4 "));

    hal::delay(clock, 1s);

    HAL_CHECK(hal::write(terminal, "Flag 5 "));

    

    // HAL_CHECK(motor.velocity_control(5.0_rpm));

    HAL_CHECK(hal::write(terminal, "Flag 5 "));

    while (true) {
        hal::delay(clock, 1s);
    }

    return hal::success();
}

}
