#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/units.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-lpc40/i2c.hpp>

#include "../hardware_map.hpp"

#include <string_view>
#include <array>
#include <cinttypes>
using namespace std::chrono_literals;
using namespace hal::literals;

namespace sjsu::arm {

hal::status application(sjsu::arm::application_framework& p_framework) {
    using namespace std::literals;
    auto& terminal = *p_framework.terminal;
    HAL_CHECK(hal::lpc40::i2c::get(2)); //need to use pca here

   
    auto& clock = *p_framework.clock;
    std::array<hal::byte, 1024> received_buffer{}; //{100
    received_buffer[0] = 'a';
    received_buffer[1] = 4;
    received_buffer[2] = '\0';

    while (true) {

        hal::delay(clock, 2000ms);

        hal::print(terminal, "restart\n");
        hal::print<200>(terminal, "result: %s\n", &received_buffer[0]);

        // auto readings = HAL_CHECK(terminal.read(received_buffer)); //.data,
        // hal::print(terminal, "something\n");
        // received_buffer[5] = 0;

        // hal::print<200>(terminal, "result: %s\n", readings.data);

    }


    return hal::success();
}


}