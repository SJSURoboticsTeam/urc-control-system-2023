
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/input_pin.hpp>

#include "../application.hpp"

namespace sjsu::drive {

hal::status application(application_framework& p_framework) 
{
    using namespace std::chrono_literals;
    using namespace hal::literals;

    

    return hal::success();
}

}
