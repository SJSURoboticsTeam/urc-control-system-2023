#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "../../platform-implementations/rmd_mcx_configurer.hpp"
#include "application.hpp"

namespace sjsu::arm {

hal::status application(sjsu::arm::application_framework& p_framework)
{

    using namespace std::chrono_literals;
    using namespace hal::literals;

    auto& can_router = *p_framework.router;
    auto& terminal = *p_framework.terminal;
    auto& clock = *p_framework.clock;

    auto motor = HAL_CHECK(mcx_configurer::create(can_router, 0x141, clock, 10ms));

    hal::print(terminal, "initialization finished\n");

    while(true) {
        float angle = HAL_CHECK(motor.get_motor_angle());
        hal::print<512>(terminal, "Angle: %f°\n", angle);
        hal::delay(clock, 100ms);
    }

    return hal::success();
}
}  // namespace sjsu::arm