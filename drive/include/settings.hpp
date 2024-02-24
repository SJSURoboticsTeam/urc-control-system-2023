#pragma once

#include <libhal/can.hpp>
#include <libhal/serial.hpp>
#include <libhal-esp8266/at.hpp>


namespace sjsu::drive {

    constexpr int terminal_uart = 0;
    constexpr size_t terminal_uart_buffer_size = 1024;
    constexpr hal::serial::settings terminal_uart_settings = {
        .baud_rate = 38400
    };


};