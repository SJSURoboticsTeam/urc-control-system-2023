#include <array>
#include <cstdio>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/i2c.hpp>
#include <libhal-util/i2c.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include "../implementations/tla2528.hpp"

#include "application.hpp"
namespace sjsu::arm_mimic {

hal::status application(application_framework& p_framework)
{
    using namespace hal::literals;

    auto& steady_clock = *p_framework.steady_clock;
    auto& uart0 = *p_framework.terminal;
    auto& i2c2 = *p_framework.i2c2;
    auto TLA2528 = tla::tla2528(i2c2, steady_clock);

    while(true) {
        hal::print<64>(uart0, "Testing!!\n");

    }
    hal::print<64>(uart0, "making TLA2528\n");

    // hal::byte channel = 0;
    hal::print<64>(uart0, "before while loop\n");
    while(true){
        // auto results = HAL_CHECK(TLA2528.read_one(channel));
        // hal::print<64>(uart0, "voltage: %d\n", results);

        auto buffer_of_results = HAL_CHECK(TLA2528.read_all());
        for(int i = 0; i < 8; i++) {
            hal::print<64>(uart0, "voltage %d: %d ", i, buffer_of_results[i]);
        }
        hal::print<64>(uart0, "\n");
    }
}
}