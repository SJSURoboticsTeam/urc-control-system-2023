#pragma once

#include <libhal/can.hpp>
#include <libhal/serial.hpp>
#include <libhal-esp8266/at.hpp>
#include <libhal-util/units.hpp>


namespace sjsu::drive {
    namespace settings {
        using namespace hal::literals;
        using namespace std::chrono_literals;

        constexpr int terminal_uart = 0;
        constexpr size_t terminal_uart_buffer_size = 1024;
        constexpr hal::serial::settings terminal_uart_settings = {
            .baud_rate = 38400
        };
        
        constexpr int can_bus = 2;
        constexpr hal::can::settings can_settings = {
            .baud_rate = 1.0_MHz, 
        };
    }

};