#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

hal::status application(Drive::hardware_map& p_map)
{

    using namespace std::chrono_literals;
    using namespace hal::literals;
    while(true) {
    HAL_CHECK(hal::write(*p_map.terminal, "Hello, World!\n"));
    HAL_CHECK(hal::delay(*p_map.steady_clock, 1000ms));
    }

  return hal::success();
}