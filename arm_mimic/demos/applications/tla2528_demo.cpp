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

#include "../applications/tla2528_demo.hpp"
#include "../../implementations/tla2528.hpp"
// #include "../implementations/degree_manipulation.hpp"
// #include "../implementations/mission_control_handler.hpp"

namespace sjsu::arm_mimic{

using namespace std::literals;

hal::status application(sjsu::arm_mimic::application_framework& p_framework) {

    auto& steady_clock = *p_framework.steady_clock;
    // auto& uart0 = *p_framework.terminal;
    auto& i2c2 = *p_framework.i2c2;
    auto TLA2528 = tla2528::tla2528(i2c2, steady_clock);
    while (true)
    {
        TLA2528.write_GPO(0,true);
        hal::delay(steady_clock,500ms);

        TLA2528.write_GPO(0,false);
        hal::delay(steady_clock,500ms);

    }
    

};

}