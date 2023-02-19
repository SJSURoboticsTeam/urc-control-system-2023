#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include <string_view>

#include "../hardware_map.hpp"


std::string_view BoolToString(bool magnet_reading) {
    if(magnet_reading) {
        return "true";
    }
    return "false";
}

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

    auto& terminal = *p_map.terminal;
    auto& counter = *p_map.steady_clock;
    auto& magnet0 = *p_map.in_pin0;
    auto& magnet1 = *p_map.in_pin1;
    auto& magnet2 = *p_map.in_pin2;
    // HAL_CHECK(hal::write(terminal, "Starting Magnet Demo..."));
    hal::print(terminal, "Starting Magnet Demo...");
    while (true)
    {
        std::string_view mag0 = BoolToString(HAL_CHECK(magnet0.level()));
          hal::print<20>(terminal, "R: %.*s | ", mag0.size(), mag0.data());
          HAL_CHECK(hal::delay(counter, 500ms));
        std::string_view mag1 = BoolToString(HAL_CHECK(magnet1.level()));
          hal::print<20>(terminal, "B: %.*s | ", mag1.size(), mag1.data());
          HAL_CHECK(hal::delay(counter, 500ms));
        std::string_view mag2 = BoolToString(HAL_CHECK(magnet2.level()));
          hal::print<20>(terminal, "L: %.*s | ", mag2.size(), mag2.data());
        HAL_CHECK(hal::delay(counter, 500ms));
    }
}